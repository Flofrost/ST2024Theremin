#include "WaveManager.h"


FRESULT wave_loadFile(WaveFile* fileStruct, const char* fileName){
    FRESULT fr;
    uint dump;

    
    f_close(&fileStruct->audioFile);

    fr = f_open(&fileStruct->audioFile, fileName, FA_READ);
    if(fr) return fr;
    
    f_lseek(&fileStruct->audioFile, 0x28);
    fr = f_read(&fileStruct->audioFile, &fileStruct->fileSize, sizeof(fileStruct->fileSize), &dump);
    if(fr) return fr;
    
    f_lseek(&fileStruct->audioFile, 0x2C);
    fr = f_read(&fileStruct->audioFile, fileStruct->buffer, sizeof(fileStruct->buffer), &dump);
    if(fr) return fr;
    
    fileStruct->bufferIndex = 0;
    fileStruct->fileIndex = 0;
    fileStruct->bufferHalf = 0;
    
    return FR_OK;
}

uint8_t wave_readSample(WaveFile* fileStruct){
    uint8_t sample;
    
    if(fileStruct->bufferHalf) sample = fileStruct->buffer[fileStruct->bufferIndex + 512];
    else                       sample = fileStruct->buffer[fileStruct->bufferIndex];

    fileStruct->bufferIndex++;
    if((fileStruct->bufferIndex >= 512) || (fileStruct->bufferIndex + fileStruct->fileIndex >= fileStruct->fileSize)){
        multicore_fifo_push_timeout_us((uint) fileStruct, 1);
    }
    
    return sample;
}

FRESULT wave_fillBuffer(WaveFile* fileStruct){
    FRESULT fr;
    uint dump;

    fileStruct->fileIndex += 512;
    fileStruct->bufferHalf = !fileStruct->bufferHalf;
    fileStruct->bufferIndex = 0;
    
    if(fileStruct->fileIndex >= fileStruct->fileSize){
        f_lseek(&fileStruct->audioFile, 0x2C);
        fileStruct->fileIndex = 0;
    }

    if(fileStruct->bufferHalf) fr = f_read(&fileStruct->audioFile, &fileStruct->buffer[0], 512, &dump);
    else                       fr = f_read(&fileStruct->audioFile, &fileStruct->buffer[512], 512, &dump);

    if(fr) return fr;

    return FR_OK;
}
