#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
 
// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN 28  // you can change this to whatever you like

/* 
 * This include brings in static arrays which contain audio samples. 
 * if you want to know how to make these please see the python code
 * for converting audio samples into static arrays. 
 */
#include "sample.h"
int wav_position = 0;

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */

typedef struct Voice {
    uint32_t  phase;
    uint32_t  frequency;
    uint32_t* samples;
    uint32_t  amplitude;
} Voice;

Voice voice = {
    .phase = 0,
    .frequency = 0,
    .samples = sineSamples,
    .amplitude = 1024
};


void pwm_interrupt_handler() {
    pwm_set_gpio_level(AUDIO_PIN, voice.samples[voice.phase >> 22]);  
    voice.phase += voice.frequency;
}

//timer qui génère la pwm
int main(void) {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
    stdio_init_all();
    // set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);


    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 1.0f); 
    pwm_config_set_wrap(&config, 1024); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 512);

    struct repeating_timer timer;
    add_repeating_timer_us(-22, pwm_interrupt_handler, NULL, &timer);
    
    while(1);
}
