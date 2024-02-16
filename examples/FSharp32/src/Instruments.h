#ifndef INSTRUMENTS_H
#define INSTRUMENTS_H

#include <avr/pgmspace.h>

typedef struct Instrument{
    int8_t samples[256];
    int8_t name[16];
    int8_t icon;
} Instrument;

#define N_INSTRUMENTS 11
extern const Instrument instrumentList[N_INSTRUMENTS] PROGMEM;

extern const uint16_t noteFrequencies[4][32] PROGMEM;
extern const int8_t noteNames[4][32][4] PROGMEM;
extern const int8_t frequencyStrings[4][32][5] PROGMEM;

extern Instrument loadedInstrument;

void loadInstrument(uint8_t instrumentIndex);

#endif