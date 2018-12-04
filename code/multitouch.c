#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define serial_pin_out (1 << PA5)
#define PIN_LED PA7
#define ADDRESS 1
#define TIMEOUT_TIME 5000

uint8_t scan_pad(
        volatile unsigned char* sendport,
        volatile unsigned char* sendddr,
        unsigned char sendpin,
        unsigned char muxval) {  
    // Wait for the receive pin to settle
    ADMUX = muxval;
    
    int temp_value = 100;
    while (temp_value > 10) {
        clear(PRR, PRADC);
        set(ADCSRA, ADSC);
        while (ADCSRA & (1 << ADSC));
        temp_value = ADCL;
        temp_value += ADCH << 8;
    }

    // Make receive pins inputs
    clear(DDRA, PA0);
    
    // Activate the first column
    set(*sendddr, sendpin);
    set(*sendport, sendpin);
    
    // Get an analog read
    _delay_us(1);
    clear(PRR, PRADC);
    set(ADCSRA, ADSC);
    while (ADCSRA & (1 << ADSC));
    put_char(&serial_port, serial_pin_out, ADCH);
    
    clear(*sendport, sendpin);
    clear(*sendddr, sendpin);

    // Make receive pins outputs to quickly bleed off charge.
    set(DDRA, PA0);
    return ADCH < 10;
    // _delay_us(1);
}

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB |= (1 << ADLAR);

    // Initialize various special features
    serial_init(serial_pin_out);
    output(DDRA, PIN_LED);
    set(PORTA, PIN_LED);
    
    static unsigned char send_pins[3] = {PA3, PA2, PA1};
    
    // specify which pin to use as the ADC
    static unsigned char receive_muxes[1] = {0b00000000};
    
    // for some reason, all outputs only show up when shooting out of PA3
    while(1) {
        clear(PORTA, PIN_LED);
        _delay_ms(200);
        set(PORTA, PIN_LED);
        _delay_ms(200);
        continue;
        put_char(&serial_port, serial_pin_out, 'h');
        put_char(&serial_port, serial_pin_out, 'e');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'o');
        put_char(&serial_port, serial_pin_out, ADDRESS);
        uint8_t pressed1 = scan_pad(&PORTA, &DDRA, send_pins[0], receive_muxes[0]);
        uint8_t pressed2 = scan_pad(&PORTA, &DDRA, send_pins[1], receive_muxes[0]);
        uint8_t pressed3 = scan_pad(&PORTA, &DDRA, send_pins[2], receive_muxes[0]);
        if (pressed1 || pressed2 || pressed3) {
            set(PORTA, PIN_LED);
        }
        else {
            clear(PORTA, PIN_LED);
        }
    }
}
