// Originally by
// Neil Gershenfeld
// 12/8/10
//
// Modified beyond recognition by Matt Keeter
//

#ifndef SERIAL_H
#define SERIAL_H

#include <avr/pgmspace.h>

void put_char_no_delay(volatile unsigned char *port, unsigned char pin, char txchar);
void put_char(volatile unsigned char *port, unsigned char pin, char txchar);
void get_char_interrupt(volatile unsigned char *pins, unsigned char pin, char* rxbyte, unsigned char* interrupt);
void get_char_timeout(volatile unsigned char *pins, unsigned char pin, char *rxbyte, uint32_t timeout);
void get_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte);

#endif
