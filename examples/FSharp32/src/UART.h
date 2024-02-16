#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define UART_SEND_BYTE(x) while(!(UCSR0A & 0x20)); UDR0 = (x)
#define UART_SEND_STR(x)  for(uint8_t i = 0 ; x[i] ; i++){UART_SEND_BYTE(x[i]);}

#endif