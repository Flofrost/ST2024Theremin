#ifndef INPUTS_H
#define INPUTS_H

#include <avr/interrupt.h>
#include "fSharp32.h"
#include "SSD1306.h"
#include "Instruments.h"

#define PHASE_A PIND & 0x04
#define PHASE_B PIND & 0x08

#define VIBRATO !(PINC & 0x10)
#define TREMOLO !(PINC & 0x20)

#define MENU !(PINC & 0x40)
#define BACK !(PINC & 0x80)

#define OCTAVE ((PINC >> 2) & 0x03)

#define N_KEYS 32

extern volatile int8_t incrementsModulator;
extern uint32_t keyboardState, keyboardPreviousState;

extern volatile uint8_t menuButton, backButton;


void (*keyboardHandlingFunction)();

void readKeyboard();

void normalKeyboardOperation();
void toggleKeyboardOperation();
void burstKeyboardOperation();

#endif