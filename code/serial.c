//
// serial.c
// based on hello.ftdi.44.echo.c by Neil Gershenfeld
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"

// We're using Neil's macros here (rather than mine)
// (I dislike these macros, because they require explicit shifting - but that
//  useful for fast serial stuff since you only need to calculate the shift
//  once).
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define output(directions,pin) (directions |= pin) // set port direction for output
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

#define bit_delay_time 8.5 // bit delay for 115200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay
#define char_delay() _delay_ms(10) // char delay

void serial_init(void)
{
   //
   // initialize output pins
   //
   set(serial_port, serial_pin_out);
   output(serial_direction, serial_pin_out);
}

void put_char(volatile unsigned char *port, unsigned char pin, char txchar) {
   //
   // send character in txchar on port pin
   //    assumes line driver (inverts bits)
   //
   // start bit
   //
   clear(*port,pin);
   bit_delay();
   //
   // unrolled loop to write data bits
   //
   if bit_test(txchar,0)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,1)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,2)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,3)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,4)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,5)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,6)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   if bit_test(txchar,7)
      set(*port,pin);
   else
      clear(*port,pin);
   bit_delay();
   //
   // stop bit
   //
   set(*port,pin);
   bit_delay();
   //
   // char delay
   //
   bit_delay();

   }

void put_int(volatile unsigned char *port, unsigned char pin, int x) {

    if (x == 0) {
        put_char(port, pin, '0');
        return;
    }
      
    
    // Loop until we exceed the value to be printed
    int index = 1;
    for (index = 1; index <= x; index *= 10);
  
    while(index > 1) {
         index /= 10;
         put_char(port, pin, '0' + x / index);
         x = x % index;
    }
}

void put_flash_string(volatile unsigned char *port, unsigned char pin, PGM_P str) {
   //
   // print a null-terminated string from flash
   //
   static char chr;
   static int index;
   index = 0;
   do {
      chr = pgm_read_byte(&(str[index]));
      put_char(port, pin, chr);
      ++index;
      } while (chr != 0);
   }

void put_ram_string(volatile unsigned char *port, unsigned char pin, char *str) {
   //
   // print a null-terminated string from SRAM
   //
   static int index;
   index = 0;
   do {
      put_char(port, pin, str[index]);
      ++index;
      } while (str[index] != 0);
   }
