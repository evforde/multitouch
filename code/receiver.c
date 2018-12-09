#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define serial_pin_out PA0
#define serial_pin_out_sft (1 << serial_pin_out)
#define PIN_LED PA7
#define ADDRESS 0
#define TIMEOUT_TIME 5000

#define MAX_BUFFER 9

static unsigned char led_pins[3] = {PA3, PA2, PA1};

ISR(PCINT0_vect) {
    static char chr;
    static char buffer[MAX_BUFFER] = {0};
    static int index = 0;
    get_char(&serial_pins, MISO, &chr);
    put_char_no_delay(&serial_port, serial_pin_out_sft, chr);
    if (chr == 'h' || index == MAX_BUFFER) {
        index = 0;
    }
    buffer[index++] = chr;
    // Check for start pattern
    if (buffer[0] == 'h' && buffer[1] == 'e' && buffer[2] == 'l' &&
        buffer[3] == 'l' && buffer[4] == 'o' && index == 9) {
        if (buffer[6])
            set(PORTA, led_pins[0]);
        else
            clear(PORTA, led_pins[0]);
        if (buffer[7])
            set(PORTA, led_pins[1]);
        else
            clear(PORTA, led_pins[1]);
        if (buffer[8])
            set(PORTA, led_pins[2]);
        else
            clear(PORTA, led_pins[2]);
    }
}

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
    // set(PORTA, led_pins[0]);
    //clear(PORTA, PIN_LED);
    output(DDRA, MOSI);
    GIMSK |= 1 << PCIE0;
    PCMSK0 |= 1 << MISO;
    sei();
    
    put_char(&serial_port, serial_pin_out_sft, 'h');
    while(1) {
        continue;
        // read and echo bits
        if bit_test(PINA, MISO)
            set(PORTA, serial_pin_out);
        else
            clear(PORTA, serial_pin_out);
    }
}
