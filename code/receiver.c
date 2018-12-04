#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define serial_pin_out (1 << PA0)
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
    serial_init(serial_pin_out);
    output(DDRA, PIN_LED);
    set(PORTA, PIN_LED);
    int i = 0;
    for (i = 0; i < 3; i++) {
        output(DDRA, led_pins[i]);
        set(PORTA, led_pins[i]);
    }
    clear(PORTA, led_pins[0]);
    
    while(1) {
        // Echo serial over USB
        if (1) {
            if bit_test(PINA, PA5)
                set(PORTA, PA0);
            else
                clear(PORTA, PA0);
        }
        else {
            // Read from serial
            static char chr;
            static char buffer[MAX_BUFFER] = {0};
            static int index = 0;
            get_char(&serial_pins, 1 << PA5, &chr);
            if (chr == 104 || index == MAX_BUFFER) {
                index = 0;
            }
            buffer[index++] = chr;
            // Check for start pattern
            /*if (buffer[0] == 'h' && buffer[1] == 'e' && buffer[2] == 'l' &&
                buffer[3] == 'l' && buffer[4] == 'o') {*/
            if (buffer[0] == 104){
                set(PORTA, led_pins[0]);
                _delay_ms(1000);
                clear(PORTA, led_pins[0]);
                _delay_ms(1000);
            }
        }
    }
}
