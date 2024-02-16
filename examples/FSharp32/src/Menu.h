#ifndef MENU_H
#define MENU_H

#include "fSharp32.h"
#include "Inputs.h"
#include "Instruments.h"
#include "SSD1306.h"


extern void (*screenControlFunction)();

void mainScreenInit();
void mainScreenController();

void mainMenuInit();
void mainMenuController();

void instrumentMenuInit();
void instrumentMenuController();

void keyboardModeMenuInit();
void keyboardModeMenuController();

void envelopeMenuInit();
void envelopeMenuController();

void valueEditInit(uint8_t* valuePtr, uint8_t x, uint8_t y, void (*returnTo)(), uint8_t flags);
void valueEditController();

void attackMenuInit();
void attackMenuController();

void decayMenuInit();
void decayMenuController();

void sustainMenuInit();
void sustainMenuController();

void releaseMenuInit();
void releaseMenuController();

void profileMenuInit();
void profileMenuController();

void profileOptionsMenuInit(uint8_t profileIndex);
void profileOptionsMenuController();

#endif