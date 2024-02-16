#ifndef FSHARP32_H
#define FSHARP32_H

#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "Inputs.h"
#include "Menu.h"
#include "SSD1306.h"
#include "Instruments.h"

#define N_VOICES 4


typedef enum NoteStage {
    off,
    attack,
    decay,
    sustain,
    release
} NoteStage;

typedef struct Voice{
    uint16_t  phase;
    uint16_t  frequency;
    uint8_t   amplitude;
    uint8_t   originatorKey;
    uint8_t   counter;
    NoteStage stage;
} Voice;

typedef struct Envelope{
    uint8_t attackTarget;
    uint8_t attackStep;
    uint8_t attackDelay;
    uint8_t decayTarget;
    uint8_t decayStep;
    uint8_t decayDelay;
    uint8_t sustainStep;
    uint8_t sustainDelay;
    uint8_t releaseStep;
    uint8_t releaseDelay;
} Envelope;

typedef struct Profile{
    Envelope envelope;
    uint8_t  selectedInstrument;
    int8_t   name[16];
} Profile;

extern Profile EEMEM savedProfiles[100];

extern uint8_t octave;

extern uint8_t keyToVoiceMap[32];
extern Voice voices[N_VOICES];

extern Profile loadedProfile;
extern uint8_t loadedKeyboardMode;

uint8_t allocateVoice();

void loadProfile(uint8_t profileIndex);
void saveProfile(uint8_t profileIndex);


#endif