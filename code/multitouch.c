#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//#include "serial.h"
#include "macros.h"

#define PIN_LED PA7
#define TIMEOUT_TIME 5000

void blink2(void) {
    set(PORTA, PIN_LED);
    _delay_ms(200);
    clear(PORTA, PIN_LED);
    _delay_ms(200);
    set(PORTA, PIN_LED);
    _delay_ms(200);
    clear(PORTA, PIN_LED);
    _delay_ms(200);
}

void scan_pad(
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
    //put_char(&serial_port, serial_pin_out, ADCH);
    if (ADCL < 247) {
        set(PORTA, PIN_LED);
    }
    else {
    }
    
    clear(*sendport, sendpin);
    clear(*sendddr, sendpin);

    // Make receive pins outputs to quickly bleed off charge.
    set(DDRA, PA0);
    _delay_ms(1);
}

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // ADCSRB |= (1 << ADLAR);

    // Initialize various special features
    serial_init();
    output(DDRA, PIN_LED);
    set(PORTA, PIN_LED);
    
    static unsigned char send_pins[3] = {PA3, PA2, PA1};
    
    // specify which pin to use as the ADC
    static unsigned char receive_muxes[1] = {0b00000000};
    
    // for some reason, all outputs only show up when shooting out of PA3
    while(1) {
        /*put_char(&serial_port, serial_pin_out, 'h');
        put_char(&serial_port, serial_pin_out, 'e');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'o');*/
        blink2();
        scan_pad(&PORTA, &DDRA, send_pins[1], receive_muxes[0]);
        _delay_ms(400);
        /*int i = 0;
        for (i = 0; i < 3; i++) {
        }*/
        // _delay_ms(12.1);
        // _delay_ms(3.76);
    }
}
