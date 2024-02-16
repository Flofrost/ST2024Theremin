#include "Menu.h"

uint8_t menuButtonPrevious = 0, backButtonPrevious = 0, displayIndex = 0, menuIndex = 0, screenStart = 0;
uint8_t editeeX, editeeY, editeeATR, editeeBuffer;
uint8_t* editeePTR;

void (*screenControlFunction)() = mainScreenController;
void (*returnToController)();

#define sizeofList(item) (sizeof(item) / sizeof(item[0]))
#define abs(x) ((x) < 0) ? -(x) : (x)

const int8_t mainMenuItems[][18] PROGMEM = {
    "Instruments   ",
    "Keyboard Modes",
    "Envelope      ",
    "Profiles      "
};
const int8_t keyboardModeMenuItems[][7] PROGMEM = {
    "Normal",
    "Toggle",
    "Burst "
};
const int8_t envelopeMenuItems[][8] PROGMEM = {
    "Attack ",
    "Decay  ",
    "Sustain",
    "Release"
};
const int8_t ADSRMenuItems[3][8] PROGMEM = {
    "Target ",
    "Step   ",
    "Delay  "
};
const int8_t profileOptsMenuItems[][5] PROGMEM = {
    "Load",
    "Save"
};


void mainScreenInit(){
    cli();

    screenControlFunction = mainScreenController;

    clear_SSD1306();
    printStr_SSD1306(0, 0, "Octave : ", 0);
    printStr_SSD1306(0, 4, "Freq : ", 0);
    printStr_SSD1306(0, 6, "Note : ", 0);

    switch(octave){
        case 0:
            printStr_SSD1306(9, 0, "34", 0);
            break;
        case 1:
            printStr_SSD1306(9, 0, "45", 0);
            break;
        case 2:
            printStr_SSD1306(9, 0, "56", 0);
            break;
        case 3:
            printStr_SSD1306(9, 0, "67", 0);
            break;
    }
    
    int8_t strBuff[7];
    printStr_SSD1306(0, 1, "Mode : ", 0);
    for(uint8_t i = 0 ; i < 7 ; i++) strBuff[i] = pgm_read_byte(&keyboardModeMenuItems[loadedKeyboardMode][i]);
    printStr_SSD1306(7, 1, strBuff, 0);

    printStr_SSD1306(0, 2, "Ins : ", 0);
    printStr_SSD1306(6, 2, loadedInstrument.name, 0);

    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void mainScreenController(){
    static uint8_t previousOctave = 255;

    if(octave != previousOctave){
        switch(octave){
            case 0:
                printStr_SSD1306(9, 0, "34", 0);
                break;
            case 1:
                printStr_SSD1306(9, 0, "45", 0);
                break;
            case 2:
                printStr_SSD1306(9, 0, "56", 0);
                break;
            case 3:
                printStr_SSD1306(9, 0, "67", 0);
                break;
        }
        
        previousOctave = octave;
    }
    
    uint8_t voiceIndex = 255;
    for(uint8_t i = 0 ; i < N_VOICES ; i++)
        if(voices[i].stage != off){
            voiceIndex = i;
            break;
        }

    if(voiceIndex != 255){
        int8_t frequencyString[5], noteString[4];

        frequencyString[0] = pgm_read_byte(&frequencyStrings[octave][voices[voiceIndex].originatorKey][0]);
        noteString[0]      = pgm_read_byte(&noteNames       [octave][voices[voiceIndex].originatorKey][0]);
        frequencyString[1] = pgm_read_byte(&frequencyStrings[octave][voices[voiceIndex].originatorKey][1]);
        noteString[1]      = pgm_read_byte(&noteNames       [octave][voices[voiceIndex].originatorKey][1]);
        frequencyString[2] = pgm_read_byte(&frequencyStrings[octave][voices[voiceIndex].originatorKey][2]);
        noteString[2]      = pgm_read_byte(&noteNames       [octave][voices[voiceIndex].originatorKey][2]);
        frequencyString[3] = pgm_read_byte(&frequencyStrings[octave][voices[voiceIndex].originatorKey][3]);
        noteString[3]      = pgm_read_byte(&noteNames       [octave][voices[voiceIndex].originatorKey][3]);
        frequencyString[4] = pgm_read_byte(&frequencyStrings[octave][voices[voiceIndex].originatorKey][4]);

        printStrBig_SSD1306(4, 4, frequencyString, 0);
        printStrBig_SSD1306(4, 6, noteString, 0);
    }else{
        printStrBig_SSD1306(4, 4, "    ", 0);
        printStrBig_SSD1306(4, 6, "   ", 0);
    }
    
    if(menuButton != menuButtonPrevious){
        if(menuButton) mainMenuInit();
        menuButtonPrevious = menuButton;
    }

    if(backButton != backButtonPrevious){
        if(backButton){
            voices[0].stage = off;
            voices[1].stage = off;
            voices[2].stage = off;
            voices[3].stage = off;
            for(uint8_t i = 0 ; i < N_KEYS ; i++) keyToVoiceMap[i] = 255;
        }
        backButtonPrevious = backButton;
    }
}


void mainMenuInit(){
    cli();

    screenControlFunction = mainMenuController;

    clear_SSD1306();
    printStr_SSD1306(6, 0, "Main Menu", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;
    
    sei();
}

void mainMenuController(){
    for(uint8_t i = 0 ; i < sizeofList(mainMenuItems) ; i++){
        int8_t strBuff[18];
        for(uint8_t j = 0 ; j < 18 ; j++) strBuff[j] = pgm_read_byte(&mainMenuItems[i][j]);
        printStr_SSD1306(0, i + 1, strBuff, i == menuIndex);
    }
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(mainMenuItems)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(mainMenuItems)) menuIndex = sizeofList(mainMenuItems) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) mainScreenInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            switch(menuIndex){
                case 0:
                    instrumentMenuInit();
                    break;
                case 1:
                    keyboardModeMenuInit();
                    break;
                case 2:
                    envelopeMenuInit();
                    break;
                case 3:
                    profileMenuInit();
                    break;
            }
        }
        menuButtonPrevious = menuButton;
    }
}


void instrumentMenuInit(){
    cli();

    screenControlFunction = instrumentMenuController;

    clear_SSD1306();
    printStr_SSD1306(2, 0, "Instrument Select", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;
    screenStart = 0;
    displayIndex = 0;

    sei();
}

void instrumentMenuController(){
    if(menuIndex < screenStart) screenStart = menuIndex;
    if(menuIndex > screenStart + 6) screenStart = menuIndex - 6;

        int8_t strBuff[16];
        for(uint8_t j = 0 ; j < 16 ; j++) strBuff[j] = pgm_read_byte(&instrumentList[screenStart + displayIndex].name[j]);
        if(screenStart + displayIndex == menuIndex){
            printChar_SSD1306(0, displayIndex + 1, pgm_read_byte(&instrumentList[screenStart + displayIndex].icon), ATTR_INVERTED);
            printChar_SSD1306(1, displayIndex + 1, ' ', ATTR_INVERTED);
            printStr_SSD1306 (2, displayIndex + 1, strBuff, ATTR_INVERTED);
        }else{
            printChar_SSD1306(0, displayIndex + 1, pgm_read_byte(&instrumentList[screenStart + displayIndex].icon), 0);
            printChar_SSD1306(1, displayIndex + 1, ' ', 0);
            printStr_SSD1306 (2, displayIndex + 1, strBuff, 0);
        }

    if(++displayIndex >= 7) displayIndex = 0;
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= N_INSTRUMENTS) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= N_INSTRUMENTS) menuIndex = N_INSTRUMENTS - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) mainMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            loadedProfile.selectedInstrument = menuIndex;
            loadInstrument(menuIndex);
            mainScreenInit();
        }
        menuButtonPrevious = menuButton;
    }
}


void keyboardModeMenuInit(){
    cli();

    screenControlFunction = keyboardModeMenuController;

    clear_SSD1306();
    printStr_SSD1306(4, 0, "Keyboard Mode", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void keyboardModeMenuController(){
    for(uint8_t i = 0 ; i < sizeofList(keyboardModeMenuItems) ; i++){
        int8_t strBuff[7];
        for(uint8_t j = 0 ; j < 7 ; j++) strBuff[j] = pgm_read_byte(&keyboardModeMenuItems[i][j]);
            printStr_SSD1306(0, i+1, strBuff, i == menuIndex);
    }
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(keyboardModeMenuItems)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(keyboardModeMenuItems)) menuIndex = sizeofList(keyboardModeMenuItems) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) mainMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            loadedKeyboardMode = menuIndex;
            
            if     (loadedKeyboardMode == 0) keyboardHandlingFunction = normalKeyboardOperation;
            else if(loadedKeyboardMode == 1) keyboardHandlingFunction = toggleKeyboardOperation;
            else if(loadedKeyboardMode == 2) keyboardHandlingFunction = burstKeyboardOperation;

            mainScreenInit();
        }
        menuButtonPrevious = menuButton;
    }
}


void envelopeMenuInit(){
    cli();

    screenControlFunction = envelopeMenuController;

    clear_SSD1306();
    printStr_SSD1306(1, 0, "Envelope Generator", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void envelopeMenuController(){
    for(uint8_t i = 0 ; i < sizeofList(envelopeMenuItems) ; i++){
        int8_t strBuff[8];
        for(uint8_t j = 0 ; j < 8 ; j++) strBuff[j] = pgm_read_byte(&envelopeMenuItems[i][j]);
        printStr_SSD1306(0, i+1, strBuff, i == menuIndex);
    }
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(envelopeMenuItems)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(envelopeMenuItems)) menuIndex = sizeofList(envelopeMenuItems) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) mainMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            switch(menuIndex){
                case 0:
                    attackMenuInit();
                    break;
                case 1:
                    decayMenuInit();
                    break;
                case 2:
                    sustainMenuInit();
                    break;
                case 3:
                    releaseMenuInit();
                    break;
            }
        }
        menuButtonPrevious = menuButton;
    }
}


void valueEditInit(uint8_t* valuePtr, uint8_t x, uint8_t y, void (*returnTo)(), uint8_t flags){
    cli();

    screenControlFunction = valueEditController;

    editeeX = x;
    editeeY = y;
    editeeATR = flags;
    editeePTR = valuePtr;
    editeeBuffer = *valuePtr;
    returnToController = returnTo;
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;

    sei();
}

void valueEditController(){
    
    printUInt8_SSD1306(editeeX, editeeY, editeeBuffer, ' ', editeeATR);

    if(incrementsModulator < -2){
        editeeBuffer--;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        editeeBuffer++;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) returnToController();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            *editeePTR = editeeBuffer;
            returnToController();
        }
        menuButtonPrevious = menuButton;
    }
}


void attackMenuInit(){
    cli();

    screenControlFunction = attackMenuController;

    clear_SSD1306();
    printStr_SSD1306(3, 0, "Attack Settings", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void attackMenuController(){
    for(uint8_t i = 0 ; i < 3 ; i++){
        int8_t strBuff[8];
        for(uint8_t j = 0 ; j < 8 ; j++) strBuff[j] = pgm_read_byte(&ADSRMenuItems[i][j]);
        if(i == menuIndex){
            printStr_SSD1306(0, i+1, strBuff, ATTR_INVERTED);
            if     (i == 0) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackTarget, ' ', ATTR_INVERTED);
            else if(i == 1) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackStep,   ' ', ATTR_INVERTED);
            else if(i == 2) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackDelay,  ' ', ATTR_INVERTED);
        }else{
            printStr_SSD1306        (0, i+1, strBuff, 0);
            if     (i == 0) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackTarget, ' ', 0);
            else if(i == 1) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackStep,   ' ', 0);
            else if(i == 2) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.attackDelay,  ' ', 0);
        }
    }
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(ADSRMenuItems)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(ADSRMenuItems)) menuIndex = sizeofList(ADSRMenuItems) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) envelopeMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            if     (menuIndex == 0) valueEditInit(&loadedProfile.envelope.attackTarget, 7, 1, attackMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else if(menuIndex == 1) valueEditInit(&loadedProfile.envelope.attackStep,   7, 2, attackMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else if(menuIndex == 2) valueEditInit(&loadedProfile.envelope.attackDelay,  7, 3, attackMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
        }
        menuButtonPrevious = menuButton;
    }
}


void decayMenuInit(){
    cli();

    screenControlFunction = decayMenuController;

    clear_SSD1306();
    printStr_SSD1306(4, 0, "Decay Settings", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void decayMenuController(){
    for(uint8_t i = 0 ; i < 3 ; i++){
        int8_t strBuff[8];
        for(uint8_t j = 0 ; j < 8 ; j++) strBuff[j] = pgm_read_byte(&ADSRMenuItems[i][j]);
        if(i == menuIndex){
            printStr_SSD1306(0, i+1, strBuff, ATTR_INVERTED);
            if     (i == 0) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayTarget, ' ', ATTR_INVERTED);
            else if(i == 1) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayStep,   ' ', ATTR_INVERTED);
            else if(i == 2) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayDelay,  ' ', ATTR_INVERTED);
        }else{
            printStr_SSD1306        (0, i+1, strBuff, 0);
            if     (i == 0) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayTarget, ' ', 0);
            else if(i == 1) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayStep,   ' ', 0);
            else if(i == 2) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.decayDelay,  ' ', 0);
        }
    }
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(ADSRMenuItems)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(ADSRMenuItems)) menuIndex = sizeofList(ADSRMenuItems) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) envelopeMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            if     (menuIndex == 0) valueEditInit(&loadedProfile.envelope.decayTarget, 7, 1, decayMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else if(menuIndex == 1) valueEditInit(&loadedProfile.envelope.decayStep,   7, 2, decayMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else if(menuIndex == 2) valueEditInit(&loadedProfile.envelope.decayDelay,  7, 3, decayMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
        }
        menuButtonPrevious = menuButton;
    }
}


void sustainMenuInit(){
    cli();

    screenControlFunction = sustainMenuController;

    clear_SSD1306();
    printStr_SSD1306(2, 0, "Sustain Settings", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void sustainMenuController(){
    for(uint8_t i = 0 ; i < 2 ; i++){
        int8_t strBuff[8];
        for(uint8_t j = 0 ; j < 8 ; j++) strBuff[j] = pgm_read_byte(&ADSRMenuItems[i+1][j]);
        if(i == menuIndex){
            printStr_SSD1306(0, i+1, strBuff, ATTR_INVERTED);
            if(i) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.sustainDelay, ' ', ATTR_INVERTED);
            else  printUInt8_SSD1306(7, i+1, loadedProfile.envelope.sustainStep,  ' ', ATTR_INVERTED);
        }else{
            printStr_SSD1306        (0, i+1, strBuff, 0);
            if(i) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.sustainDelay, ' ', 0);
            else  printUInt8_SSD1306(7, i+1, loadedProfile.envelope.sustainStep,  ' ', 0);
        }
    }
    

    if(abs(incrementsModulator) > 2){
        menuIndex = !menuIndex;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) envelopeMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            if(menuIndex) valueEditInit(&loadedProfile.envelope.sustainDelay, 7, 2, sustainMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else          valueEditInit(&loadedProfile.envelope.sustainStep,  7, 1, sustainMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
        }
        menuButtonPrevious = menuButton;
    }
}


void releaseMenuInit(){
    cli();

    screenControlFunction = releaseMenuController;

    clear_SSD1306();
    printStr_SSD1306(2, 0, "Release Settings", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void releaseMenuController(){
    for(uint8_t i = 0 ; i < 2 ; i++){
        int8_t strBuff[8];
        for(uint8_t j = 0 ; j < 8 ; j++) strBuff[j] = pgm_read_byte(&ADSRMenuItems[i+1][j]);
        if(i == menuIndex){
            printStr_SSD1306(0, i+1, strBuff, ATTR_INVERTED);
            if(i) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.releaseDelay, ' ', ATTR_INVERTED);
            else  printUInt8_SSD1306(7, i+1, loadedProfile.envelope.releaseStep,  ' ', ATTR_INVERTED);
        }else{
            printStr_SSD1306        (0, i+1, strBuff, 0);
            if(i) printUInt8_SSD1306(7, i+1, loadedProfile.envelope.releaseDelay, ' ', 0);
            else  printUInt8_SSD1306(7, i+1, loadedProfile.envelope.releaseStep,  ' ', 0);
        }
    }
    

    if(abs(incrementsModulator) > 2){
        menuIndex = !menuIndex;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) envelopeMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            if(menuIndex) valueEditInit(&loadedProfile.envelope.releaseDelay, 7, 2, releaseMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
            else          valueEditInit(&loadedProfile.envelope.releaseStep,  7, 1, releaseMenuInit, ATTR_UNDERLINE | ATTR_INVERTED);
        }
        menuButtonPrevious = menuButton;
    }
}


void profileMenuInit(){
    cli();

    screenControlFunction = profileMenuController;

    clear_SSD1306();
    printStr_SSD1306(6, 0, "Profiles", 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;
    screenStart = 0;
    displayIndex = 0;

    sei();
}

void profileMenuController(){
    
    if(menuIndex < screenStart) screenStart = menuIndex;
    if(menuIndex > screenStart + 6) screenStart = menuIndex - 6;

    int8_t strBuff[16];
    eeprom_read_block(strBuff, savedProfiles[screenStart + displayIndex].name, 16);
    printHex8_SSD1306(0, displayIndex + 1, screenStart + displayIndex, screenStart + displayIndex == menuIndex);
    printStr_SSD1306(2, displayIndex + 1, " - ", screenStart + displayIndex == menuIndex);
    printStr_SSD1306(5, displayIndex + 1, strBuff, screenStart + displayIndex == menuIndex);
    
    if(++displayIndex >= 7) displayIndex = 0;
    

    if(incrementsModulator < -2){
        menuIndex++;
        if(menuIndex >= sizeofList(savedProfiles)) menuIndex = 0;
        incrementsModulator = 0;
    }

    if(incrementsModulator > 2){
        menuIndex--;
        if(menuIndex >= sizeofList(savedProfiles)) menuIndex = sizeofList(savedProfiles) - 1;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) mainMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton) profileOptionsMenuInit(menuIndex);
        menuButtonPrevious = menuButton;
    }
}


void profileOptionsMenuInit(uint8_t profileIndex){
    cli();

    editeeBuffer = profileIndex;
    screenControlFunction = profileOptionsMenuController;

    clear_SSD1306();
    int8_t strBuff[16];
    eeprom_read_block(strBuff, savedProfiles[profileIndex].name, 16);
    printStr_SSD1306(3, 0, strBuff, 0);
    
    menuButtonPrevious = menuButton;
    backButtonPrevious = backButton;
    menuIndex = 0;
    incrementsModulator = 0;

    sei();
}

void profileOptionsMenuController(){
    for(uint8_t i = 0 ; i < 2 ; i++){
        int8_t strBuff[5];
        for(uint8_t j = 0 ; j < 5 ; j++) strBuff[j] = pgm_read_byte(&profileOptsMenuItems[i][j]);
        printStr_SSD1306(0, i+1, strBuff, i == menuIndex);
    }
    

    if(abs(incrementsModulator) > 2){
        menuIndex = !menuIndex;
        incrementsModulator = 0;
    }

    if(backButton != backButtonPrevious){
        if(backButton) profileMenuInit();
        backButtonPrevious = backButton;
    }

    if(menuButton != menuButtonPrevious){
        if(menuButton){
            if(menuIndex) saveProfile(editeeBuffer);
            else          loadProfile(editeeBuffer);
            mainScreenInit();
        }
        menuButtonPrevious = menuButton;
    }
}
