#ifndef H_WAVEMANAGER
#define H_WAVEMANAGER

#include "pico/multicore.h"
#include "hw_config.h"
#include "sd_card.h"
#include "ff.h"


typedef struct WaveFile {
    FIL audioFile;
    uint fileSize;
    uint fileIndex;
    uint bufferIndex;
    uint bufferHalf;
    uint8_t buffer[1024];
    uint8_t playing;
    uint8_t volume;
} WaveFile;


FRESULT wave_loadFile(WaveFile* fileStruct, const char* fileName);
uint8_t wave_readSample(WaveFile* fileStruct);
FRESULT wave_fillBuffer(WaveFile* fileStruct);

#endif