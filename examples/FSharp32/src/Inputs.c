#include "Inputs.h"

volatile int8_t incrementsModulator = 0;
uint8_t readingOffset = 0;

uint32_t keyboardState = 0, keyboardPreviousState = 0;

volatile uint8_t menuButton = 0, backButton = 0;


ISR(INT0_vect){
    if(PHASE_B) incrementsModulator++;
    else        incrementsModulator--;
}

ISR(INT1_vect){
    if(PHASE_A) incrementsModulator--;
    else        incrementsModulator++;
}


// Reading Keyboard
ISR(TIMER1_OVF_vect){ // ~ 244 Hz
    sei();
    keyboardHandlingFunction();
}


void readKeyboard(){
    uint8_t keys = ~PINB;

    switch(readingOffset){
        case 0:
            keyboardState &= 0xFFFFFF00;
            keyboardState |= (uint32_t)keys;
            break;
        case 1:
            keyboardState &= 0xFFFF00FF;
            keyboardState |= (uint32_t)keys << 8;
            break;
        case 2:
            keys = (keys & 0xF0) >> 4 | (keys & 0x0F) << 4;
            keys = (keys & 0xCC) >> 2 | (keys & 0x33) << 2;
            keys = (keys & 0xAA) >> 1 | (keys & 0x55) << 1;
            keyboardState &= 0xFF00FFFF;
            keyboardState |= (uint32_t)keys << 16;
            break;
        case 3:
            keys = (keys & 0xF0) >> 4 | (keys & 0x0F) << 4;
            keys = (keys & 0xCC) >> 2 | (keys & 0x33) << 2;
            keys = (keys & 0xAA) >> 1 | (keys & 0x55) << 1;
            keyboardState &= 0x00FFFFFF;
            keyboardState |= (uint32_t)keys << 24;
            break;
    }

    readingOffset++;
    readingOffset &= 0x03;
    DDRD = 0x10 << readingOffset;
}


void normalKeyboardOperation(){
    readKeyboard();
    if(keyboardState != keyboardPreviousState){
        uint32_t presses = keyboardState ^ keyboardPreviousState;
        uint32_t releases = (~keyboardState) & presses;
        presses = keyboardState & presses;

        if(presses)
            for(uint8_t i = 0 ; i < N_KEYS ; i++)
                if((presses >> i) & 0x01){
                    keyToVoiceMap[i] = allocateVoice();
                    if(keyToVoiceMap[i] != 255){
                        voices[keyToVoiceMap[i]].stage = attack;
                        voices[keyToVoiceMap[i]].frequency = pgm_read_word(&noteFrequencies[octave][i]);
                        voices[keyToVoiceMap[i]].amplitude = 0;
                        voices[keyToVoiceMap[i]].counter = 0;
                        voices[keyToVoiceMap[i]].originatorKey = i;
                    }
                }

        if(releases)
            for(uint8_t i = 0 ; i < N_KEYS ; i++)
                if((releases >> i) & 0x01){
                    if(keyToVoiceMap[i] != 255){
                        voices[keyToVoiceMap[i]].stage = release;
                        keyToVoiceMap[i] = 255;
                    }
                }

        keyboardPreviousState = keyboardState;
    }
}

void toggleKeyboardOperation(){
    readKeyboard(&keyboardState);
    if(keyboardState != keyboardPreviousState){
        uint32_t presses = keyboardState ^ keyboardPreviousState;
        uint32_t releases = (~keyboardState) & presses;
        presses = keyboardState & presses;

        if(presses)
            for(uint8_t i = 0 ; i < N_KEYS ; i++)
                if((presses >> i) & 0x01){
                    if(keyToVoiceMap[i] == 255){
                        keyToVoiceMap[i] = allocateVoice();
                        if(keyToVoiceMap[i] != 255){
                            voices[keyToVoiceMap[i]].stage = attack;
                            voices[keyToVoiceMap[i]].frequency = pgm_read_word(&noteFrequencies[octave][i]);
                            voices[keyToVoiceMap[i]].amplitude = 0;
                            voices[keyToVoiceMap[i]].originatorKey = i;
                        }
                    }else{
                        voices[keyToVoiceMap[i]].stage = release;
                        keyToVoiceMap[i] = 255;
                    }
                }

        keyboardPreviousState = keyboardState;
    }
}

void burstKeyboardOperation(){
    readKeyboard(&keyboardState);
    if(keyboardState != keyboardPreviousState){
        uint32_t presses = keyboardState ^ keyboardPreviousState;
        uint32_t releases = (~keyboardState) & presses;
        presses = keyboardState & presses;

        if(presses)
            for(uint8_t i = 0 ; i < N_KEYS ; i++)
                if((presses >> i) & 0x01){
                    keyToVoiceMap[i] = allocateVoice();
                    if(keyToVoiceMap[i] != 255){
                        voices[keyToVoiceMap[i]].stage = release;
                        voices[keyToVoiceMap[i]].frequency = pgm_read_word(&noteFrequencies[octave][i]);
                        voices[keyToVoiceMap[i]].amplitude = loadedProfile.envelope.attackTarget;
                        voices[keyToVoiceMap[i]].originatorKey = i;
                    }
                }

        keyboardPreviousState = keyboardState;
    }
}
