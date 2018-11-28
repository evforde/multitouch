// Originally by
// Neil Gershenfeld
// 12/8/10
//
// Modified beyond recognition by Matt Keeter
//

#ifndef SERIAL_H
#define SERIAL_H

#define serial_port PORTD
#define serial_direction DDRD
#define serial_pins PIND
#define serial_pin_out (1 << PD1)

#include <avr/pgmspace.h>

void serial_init(void);
void put_char(volatile unsigned char *port, unsigned char pin, char txchar);
void put_int(volatile unsigned char *port, unsigned char pin, int x);
void put_ram_string(volatile unsigned char *port, unsigned char pin, char *str);
void put_flash_string(volatile unsigned char *port, unsigned char pin, PGM_P str);

#endif
