#include "BGM.h"


ChannelManager channels[3];


void initChannel(ChannelManager* channel, const char* dirName, uint pp_gpio, uint p_gpio, uint n_gpio, uint va_gpio, uint vb_gpio){
    channel->numberOfFiles = countFiles(dirName);
    channel->fileNames = (char*) ff_memalloc(channels[0].numberOfFiles * 256);
    loadFileNames(dirName, channel->fileNames, channel->numberOfFiles);
    
    channel->pp_gpio = pp_gpio;
    channel->p_gpio = p_gpio;
    channel->n_gpio = n_gpio;
    channel->va_gpio = va_gpio;
    channel->vb_gpio = vb_gpio;

    initBinaryInputController(&channel->playPauseButton, time_us_64(), 100000);
    initBinaryInputController(&channel->prevButton, time_us_64(), 100000);
    initBinaryInputController(&channel->nextButton, time_us_64(), 100000);
    initBinaryInputController(&channel->volumeEncoder, time_us_64(), 1000);

    channel->audioChannel.playing = 1;
    channel->audioChannel.volume = 255;
}

void handleChannelControls(ChannelManager* channel){
    if(onFallingEdge(&channel->playPauseButton, gpio_get(channel->pp_gpio), time_us_64())) channel->audioChannel.playing = !channel->audioChannel.playing;

    if(onFallingEdge(&channel->prevButton, gpio_get(channel->p_gpio), time_us_64())){
        channel->selectedFile--;
        if(channel->selectedFile >= channel->numberOfFiles) channel->selectedFile = channel->numberOfFiles - 1;
        wave_loadFile(&channel->audioChannel, &channel->fileNames[channel->selectedFile * 256]);
    }

    if(onFallingEdge(&channel->nextButton, gpio_get(channel->n_gpio), time_us_64())){
        channel->selectedFile++;
        if(channel->selectedFile >= channel->numberOfFiles) channel->selectedFile = 0;        
        wave_loadFile(&channel->audioChannel, &channel->fileNames[channel->selectedFile * 256]);
    }
    
    if(onRisingEdge(&channel->volumeEncoder, gpio_get(channel->va_gpio), time_us_64())){
        if(gpio_get(channel->vb_gpio)) channel->audioChannel.volume += 5;
        else                           channel->audioChannel.volume -= 5;
    }
}


bool sampleGenerator(struct repeating_timer* timer){
    if(channels[0].audioChannel.playing) pwm_set_chan_level(0, PWM_CHAN_A, ((uint) wave_readSample(&channels[0].audioChannel) * (uint) channels[0].audioChannel.volume) >> 8);
    if(channels[1].audioChannel.playing) pwm_set_chan_level(0, PWM_CHAN_B, ((uint) wave_readSample(&channels[1].audioChannel) * (uint) channels[1].audioChannel.volume) >> 8);
    if(channels[2].audioChannel.playing) pwm_set_chan_level(1, PWM_CHAN_A, ((uint) wave_readSample(&channels[2].audioChannel) * (uint) channels[2].audioChannel.volume) >> 8);

    return 1;
}

void otherCore(){
    // WaveFile* target;
    while(1){
        if(multicore_fifo_rvalid()) wave_fillBuffer((WaveFile*) multicore_fifo_pop_blocking());
        
        handleChannelControls(&channels[0]);
        handleChannelControls(&channels[1]);
        handleChannelControls(&channels[2]);
    }
    
}

uint countFiles(const char* dirName){
    DIR dir;
    uint counter = 0;
    FILINFO fileInfo;
    
    f_opendir(&dir, dirName);
    while(1){
        f_readdir(&dir, &fileInfo);
        if(!fileInfo.fname[0]) break;
        counter++;
    }
    f_closedir(&dir);

    return counter;
}

void loadFileNames(const char* dirName, char* nameArray, uint count){
    DIR dir;
    FILINFO fi;
    
    f_opendir(&dir, dirName);
    for(uint i = 0 ; i < count ; i++){
        f_readdir(&dir, &fi);
        uint l = sprintf(&nameArray[i * 256], "%s/%s\0", dirName, fi.fname);
        printf("%p - %s - %s/%s\n", &nameArray[i * 256], &nameArray[i * 256], dirName, fi.fname);
    }
    f_closedir(&dir);
}

int main() {
    stdio_init_all();

    gpio_set_function(0, GPIO_FUNC_PWM);
    gpio_set_function(1, GPIO_FUNC_PWM);
    gpio_set_function(2, GPIO_FUNC_PWM);

    pwm_set_clkdiv(0, 1.f);
    pwm_set_wrap(0, 255);
    pwm_set_chan_level(0, PWM_CHAN_A, 127);
    pwm_set_chan_level(0, PWM_CHAN_B, 127);
    pwm_set_enabled(0, 1);

    pwm_set_clkdiv(1, 1.f);
    pwm_set_wrap(1, 255);
    pwm_set_chan_level(1, PWM_CHAN_A, 127);
    pwm_set_enabled(1, 1);
    
    for(uint i = 3 ; i < 16 ; i++){
        gpio_init(i);
        gpio_set_dir(i, 0);
        gpio_pull_up(i);
    }

    gpio_init(20);
    gpio_set_dir(20, 0);
    gpio_pull_up(20);

    gpio_init(21);
    gpio_set_dir(21, 0);
    gpio_pull_up(21);

    // while(!getchar());

    sd_card_t *pSD = sd_get_by_num(0);
    if(f_mount(&pSD->fatfs, pSD->pcName, 1) != FR_OK) printf("FS Mount Error");
    
    initChannel(&channels[0], "/1", PLAYPAUSE_0, PREV_0, NEXT_0, VOLA_0, VOLB_0);
    initChannel(&channels[1], "/2", PLAYPAUSE_1, PREV_1, NEXT_1, VOLA_1, VOLB_1);
    initChannel(&channels[2], "/3", PLAYPAUSE_2, PREV_2, NEXT_2, VOLA_2, VOLB_2);
    
    printf("\n");
    for(uint i = 0 ; i < channels[0].numberOfFiles ; i++) printf("%s\n", &channels[0].fileNames[i * 256]);
    for(uint i = 0 ; i < channels[1].numberOfFiles ; i++) printf("%s\n", &channels[1].fileNames[i * 256]);
    for(uint i = 0 ; i < channels[2].numberOfFiles ; i++) printf("%s\n", &channels[2].fileNames[i * 256]);
    
    // while(!getchar());
    
    printf("\nGO!\n");
    
    wave_loadFile(&channels[0].audioChannel, channels[0].fileNames);
    wave_loadFile(&channels[1].audioChannel, channels[1].fileNames);
    wave_loadFile(&channels[2].audioChannel, channels[2].fileNames);
    
    channels[0].audioChannel.volume = 255;
    channels[1].audioChannel.volume = 128;
    channels[2].audioChannel.volume = 20;

    multicore_launch_core1(otherCore);

    struct repeating_timer timer;
    add_repeating_timer_us(-45, sampleGenerator, NULL, &timer);

    while(1);
    
    return 0;
}