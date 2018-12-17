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

#define bit_delay_time 49.5 // bit delay for 19200 with overhead
#define bit_delay() _delay_us(bit_delay_time) // RS232 bit delay
#define half_bit_delay() _delay_us(bit_delay_time/2) // RS232 half bit delay
#define char_delay() _delay_ms(10) // char delay

#define max_buffer 25

void put_char_no_delay(volatile unsigned char *port, unsigned char pin, char txchar) {
   // send character in txchar on port pin
   //    assumes line driver (inverts bits)
   // start bit
   clear(*port,pin);
   bit_delay();
   // unrolled loop to write data bits
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
   // stop bit
   set(*port,pin);
   bit_delay();
   // no delay
}

void put_char(volatile unsigned char *port, unsigned char pin, char txchar) {
    put_char_no_delay(port, pin, txchar);
    char_delay();
}

static inline void read_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte) {
    // delay to middle of first data bit
    half_bit_delay();
    bit_delay();
    // unrolled loop to read data bits
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 0);
    else
        *rxbyte |= (0 << 0);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 1);
    else
        *rxbyte |= (0 << 1);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 2);
    else
        *rxbyte |= (0 << 2);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 3);
    else
        *rxbyte |= (0 << 3);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 4);
    else
        *rxbyte |= (0 << 4);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 5);
    else
        *rxbyte |= (0 << 5);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 6);
    else
        *rxbyte |= (0 << 6);
    bit_delay();
    if bit_test(*pins,pin)
        *rxbyte |= (1 << 7);
    else
        *rxbyte |= (0 << 7);
    // wait for stop bit
    bit_delay();
    half_bit_delay();
}

void get_char(volatile unsigned char *pins, unsigned char pin, char *rxbyte) {
    // assumes line driver (inverts bits)
    *rxbyte = 0;
    while (bit_test(*pins,pin)) // wait for start bit
        ;
    read_char(pins, pin, rxbyte);
}

void get_char_timeout(volatile unsigned char *pins, unsigned char pin, char *rxbyte, uint32_t timeout) {
    // assumes line driver (inverts bits)
    *rxbyte = 0;
    uint32_t count = 0;
    while (bit_test(*pins,pin)) {
        // wait for start bit unless we time out
        count++;
        if (count > timeout)
            return;
    }
    read_char(pins, pin, rxbyte);
}

void get_char_interrupt(volatile unsigned char *pins, unsigned char pin, char* rxbyte, unsigned char* interrupt) {
    // assumes line driver (inverts bits)
    *rxbyte = 0;
    while (bit_test(*pins,pin))
        // wait for start bit unless there's an interrupt
        if (!*interrupt)
            return;
    read_char(pins, pin, rxbyte);
}
