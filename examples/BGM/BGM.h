#ifndef BGM_H
#define BGM_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "WaveManager.h"
#include "BinaryInputManager.h"


#define PLAYPAUSE_0 3
#define PREV_0      4
#define NEXT_0      5
#define PLAYPAUSE_1 6
#define PREV_1      7
#define NEXT_1      8
#define PLAYPAUSE_2 9
#define PREV_2      10
#define NEXT_2      11
#define VOLA_0      12
#define VOLB_0      13
#define VOLA_1      14
#define VOLB_1      15
#define VOLA_2      20
#define VOLB_2      21


typedef struct ChannelManager {
    WaveFile audioChannel;
    char* fileNames;
    uint numberOfFiles;
    uint selectedFile;
    uint pp_gpio, p_gpio, n_gpio, va_gpio, vb_gpio;
    BinaryInputController playPauseButton;
    BinaryInputController prevButton;
    BinaryInputController nextButton;
    BinaryInputController volumeEncoder;
} ChannelManager;


void initChannel(ChannelManager* channel, const char* dirName, uint pp_gpio, uint p_gpio, uint n_gpio, uint va_gpio, uint vb_gpio);
void handleChannelControls(ChannelManager* channel);

uint countFiles(const char* dirName);
void loadFileNames(const char* dirName, char* nameArray, uint count);

#endif