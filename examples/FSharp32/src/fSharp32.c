#include "fSharp32.h"


uint8_t octave = 0;
volatile int8_t vibrato = 0, tremolo = 0;

uint8_t keyToVoiceMap[32];
Voice voices[N_VOICES];

uint8_t loadedKeyboardMode = 0;

Profile loadedProfile = {
    .envelope = {
        .attackTarget = 128,
        .attackStep   = 128,
        .attackDelay  = 0,
        .decayTarget  = 128,
        .decayStep    = 1,
        .decayDelay   = 0,
        .sustainStep  = 0,
        .sustainDelay = 255,
        .releaseStep  = 128,
        .releaseDelay = 0
    },
    .selectedInstrument = 0,
    .name = "Default"
};


uint8_t allocateVoice(){
    uint8_t nextBest = 255;

    for(uint8_t i = 0 ; i < N_VOICES ; i++){
        if(voices[i].stage == off) return i;
        if(voices[i].stage == release){
            if(nextBest == 255) nextBest = i;
            else if(voices[i].amplitude < voices[nextBest].amplitude) nextBest = i;
        }
    }
    
    return nextBest;
}

void loadProfile(uint8_t profileIndex){
    cli();

    uint8_t numBuff = eeprom_read_byte(&savedProfiles[profileIndex].selectedInstrument);
    loadInstrument(numBuff);
    loadedProfile.selectedInstrument = numBuff;

    eeprom_read_block(&loadedProfile.envelope, &savedProfiles[profileIndex].envelope, sizeof(Envelope));

    sei();
}

void saveProfile(uint8_t profileIndex){
    cli();

    eeprom_write_block(&loadedProfile.envelope, &savedProfiles[profileIndex].envelope, sizeof(Envelope));
    
    eeprom_write_byte(&savedProfiles[profileIndex].selectedInstrument, loadedProfile.selectedInstrument);
    
    int8_t nameBuff[16];

    nameBuff[0] = loadedInstrument.icon;

    nameBuff[1] = loadedInstrument.name[0];
    nameBuff[2] = loadedInstrument.name[1];
    nameBuff[3] = loadedInstrument.name[2];

    nameBuff[4] = pgm_read_byte(&hexChars[loadedProfile.envelope.attackStep >> 4]);
    nameBuff[5] = pgm_read_byte(&hexChars[loadedProfile.envelope.attackStep & 15]);

    nameBuff[6] = pgm_read_byte(&hexChars[loadedProfile.envelope.decayTarget >> 4]);
    nameBuff[7] = pgm_read_byte(&hexChars[loadedProfile.envelope.decayTarget & 15]);

    nameBuff[8] = pgm_read_byte(&hexChars[loadedProfile.envelope.sustainStep >> 4]);
    nameBuff[9] = pgm_read_byte(&hexChars[loadedProfile.envelope.sustainStep & 15]);

    nameBuff[10] = pgm_read_byte(&hexChars[loadedProfile.envelope.releaseStep >> 4]);
    nameBuff[11] = pgm_read_byte(&hexChars[loadedProfile.envelope.releaseStep & 15]);
    
    nameBuff[12] = 0;

    eeprom_write_block(nameBuff, &savedProfiles[profileIndex].name, 16);
    
    sei();
}


static void envelopeManager(Voice* voice){
    switch(voice->stage){
        case attack:
            if(++voice->counter >= loadedProfile.envelope.attackDelay){
                voice->amplitude += loadedProfile.envelope.attackStep;

                if(voice->amplitude >= loadedProfile.envelope.attackTarget) voice->stage = decay;
                voice->counter = 0;
            }
            break;

        case decay:
            if(++voice->counter >= loadedProfile.envelope.decayDelay){
                voice->amplitude -= loadedProfile.envelope.decayStep;

                if(voice->amplitude <= loadedProfile.envelope.decayTarget) voice->stage = sustain;
                voice->counter = 0;
            }
            break;

        case sustain:
            if(++voice->counter >= loadedProfile.envelope.sustainDelay){
                if(voice->amplitude - loadedProfile.envelope.sustainStep <= 0){
                    voice->stage = off;
                    keyToVoiceMap[voice->originatorKey] = 255;
                }
                else voice->amplitude -= loadedProfile.envelope.sustainStep;

                voice->counter = 0;
            }
            break;

        case release:
            if(++voice->counter >= loadedProfile.envelope.releaseDelay){
                if(voice->amplitude - loadedProfile.envelope.releaseStep <= 0) voice->stage = off;
                else voice->amplitude -= loadedProfile.envelope.releaseStep;

                voice->counter = 0;
            }
            break;
    }
}


// SOUND GENERATOR
ISR(TIMER2_COMPA_vect){ // 15625 Hz
    int8_t sample = 0, voiceSample;

    for(uint8_t i = 0 ; i < N_VOICES ; i++)
        if(voices[i].stage != off){
            voiceSample = loadedInstrument.samples[voices[i].phase >> 8] >> 2;
            voiceSample = (voiceSample * (voices[i].amplitude + (tremolo << 4))) >> 7;
            sample += voiceSample;

            voices[i].phase += voices[i].frequency + (vibrato << 4);
        }

    PORTA = 127 + sample;
}

// Reading Other Inputs and Screen Management
ISR(TIMER0_OVF_vect){ // ~ 61 Hz
    sei();

    octave = OCTAVE;
    menuButton = MENU;
    backButton = BACK;
    
    screenControlFunction();

    if(screenControlFunction == mainScreenController)
        if(VIBRATO){
            vibrato >>= 1;
            vibrato += incrementsModulator;
            incrementsModulator = 0;
        }else if(TREMOLO){
            tremolo >>= 1;
            tremolo += incrementsModulator;
            incrementsModulator = 0;
        }
    
    for(uint8_t i = 0 ; i < N_VOICES ; i++)
        if(voices[i].stage != off) envelopeManager(&voices[i]);

}


ISR(USART0_RX_vect){
    static uint8_t index = 0;
    loadedInstrument.samples[index] = UDR0;
    index++;
}


int main(){
    
    TCCR2A = 0x02;
    TCCR2B = 0x02;
    OCR2A  = 127;
    TIMSK2 = 0x02; // output sample at roughly 15625 Hz

    TCCR1B = 0x01; 
    TIMSK1 = 0x01; // interrput at roughly 244 Hz
    
    TCCR0B = 0x05;
    TIMSK0 = 0x01; // interrupt at roughly 61 Hz
    
    EICRA = 0x0F;
    EIMSK = 0x03; // Enable INT0 and INT1

    UCSR0B = 0x98;
    UBRR0L = 0x08; // Uart setup 115200 baud
        
    TWBR = 16;
    
    DDRA = 0xFF; // Output mode for DAC
    PORTC = 0xFC; // Pullups on most of port c for buttons and switches
    PORTB = 0xFF; // Pullups for key matrix
    
    for(uint8_t i = 0 ; i < N_VOICES ; i++){ // Init of voices default values
        voices[i].phase = 0;
        voices[i].stage = off;
    }

    for(uint8_t i = 0 ; i < N_KEYS ; i++) keyToVoiceMap[i] = 255; // Init of key to voices map

    loadInstrument(loadedProfile.selectedInstrument);
    keyboardHandlingFunction = normalKeyboardOperation;

    init_SSD1306();
    
    mainScreenInit();
    
    sei();
    
    while(1);
                  
    return 0;
}