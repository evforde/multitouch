#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define serial_pin_out_sft (1 << PA0)
#define PIN_LED PA7
#define ADDRESS 0
#define TIMEOUT_TIME 5000

#define MAX_BUFFER 9

static unsigned char led_pins[3] = {PA3, PA2, PA1};


int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // Initialize various special features
    serial_init(serial_pin_out_sft);
    output(DDRA, PIN_LED);
    set(PORTA, PIN_LED);
    output(DDRA, led_pins[0]);
    output(DDRA, led_pins[1]);
    output(DDRA, led_pins[2]);
    set(PORTA, led_pins[0]);
    //clear(PORTA, PIN_LED);
    output(DDRA, MOSI);
    
    put_char(&serial_port, serial_pin_out_sft, 'h');
    while(1) {
        // Echo serial over USB
        static char chr;
        static uint8_t a = 0;
        if (0) {
            get_char(&serial_pins, MISO, &chr);
            if (chr)
                put_char(&serial_port, serial_pin_out_sft, chr);
        }
        else if (1) {
            if bit_test(PINA, MISO) set(PORTA, PIN_LED); else clear(PORTA, PIN_LED);
            put_char(&serial_port, MOSI_SFT, 'h');
        }
        else {
            // Read from serial
            static char buffer[MAX_BUFFER] = {0};
            static int index = 0;
            put_char(&serial_port, serial_pin_out_sft, chr);
            if (chr == 'h') {
                set(PORTA, led_pins[1]);
                _delay_ms(200);
                clear(PORTA, led_pins[1]);
                _delay_ms(200);
                set(PORTA, led_pins[1]);
                _delay_ms(200);
            }
            if (chr == 101) {
                set(PORTA, led_pins[2]);
                _delay_ms(200);
                clear(PORTA, led_pins[2]);
                _delay_ms(200);
                set(PORTA, led_pins[2]);
                _delay_ms(200);
            }
            continue;
            if (chr == 'h' || index == MAX_BUFFER) {
                index = 0;
            }
            if (chr != 0) {
                clear(PORTA, PIN_LED);
            }
            buffer[index++] = chr;
            // Check for start pattern
            /*if (buffer[0] == 'h' && buffer[1] == 'e' && buffer[2] == 'l' &&
                buffer[3] == 'l' && buffer[4] == 'o') {*/
            if (buffer[0] == 'h') {
                set(PORTA, led_pins[1]);
            }
        }
    }
}
