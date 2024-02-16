#ifndef SSD1306_H
#define SSD1306_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "I2C.h"

#define ATTR_INVERTED  0x01
#define ATTR_UNDERLINE 0x02
#define ATTR_STRICKTHR 0x04

#define ADDR_W_SSD1306 0x78

#define COMMAND_SSD1306(command) \
    I2C_START(); \
    I2C_WAIT_TRASMISSION(); \
    I2C_WRITE(ADDR_W_SSD1306); \
    I2C_WAIT_TRASMISSION(); \
    I2C_WRITE(0x00); \
    I2C_WAIT_TRASMISSION(); \
    I2C_WRITE(command); \
    I2C_WAIT_TRASMISSION(); \
    I2C_STOP()


extern const int8_t hexChars[16] PROGMEM;

void init_SSD1306();
void clear_SSD1306();

// flags = 0b00000SUI; I = Invert, U = Underline, S = Strikethrough
void printChar_SSD1306(uint8_t x, uint8_t y, int8_t c, uint8_t flags);
void printStr_SSD1306(uint8_t x, uint8_t y, char* s, uint8_t flags);
void printStrBig_SSD1306(uint8_t x, uint8_t y, char* s, uint8_t flags);
void printUInt8_SSD1306(uint8_t x, uint8_t y, uint8_t v, int8_t filler, uint8_t flags);
void printHex8_SSD1306(uint8_t x, uint8_t y, uint8_t v, uint8_t flags);
void printHex16_SSD1306(uint8_t x, uint8_t y, uint16_t v, uint8_t flags);
void printHex32_SSD1306(uint8_t x, uint8_t y, uint32_t v, uint8_t flags);

#endif