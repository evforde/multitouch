// Originally by
// Neil Gershenfeld
// 12/8/10
//
// Modified beyond recognition by Matt Keeter
//

#ifndef SERIAL_H
#define SERIAL_H

#define serial_port PORTA
#define serial_direction DDRA
#define serial_pins PINA

#define MISO PA5
#define MISO_SFT (1 << MISO)
#define MOSI PA6
#define MOSI_SFT (1 << MOSI)
// TODO

#include <avr/pgmspace.h>

void serial_init(unsigned char serial_pin_out);
void put_char(volatile unsigned char *port, unsigned char pin, char txchar);
void get_char_interrupt(volatile unsigned char *pins, unsigned char pin, char* rxbyte, unsigned char* interrupt);
void get_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte);
void put_int(volatile unsigned char *port, unsigned char pin, int x);
void put_ram_string(volatile unsigned char *port, unsigned char pin, char *str);
void put_flash_string(volatile unsigned char *port, unsigned char pin, PGM_P str);

#endif
