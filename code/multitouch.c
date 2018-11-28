#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define TIMEOUT_TIME 5000

static volatile unsigned char* const send_ports[12] = {&PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTB, &PORTB, &PORTB, &PORTC, &PORTC};
static volatile unsigned char* const send_ddrs[12] = {&DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRB, &DDRB, &DDRB, &DDRC, &DDRC};
static unsigned const char const send_pins[12] = {PD0, PD2, PD3, PD4, PD5, PD6, PD7, PB0, PB1, PB2, PC0, PC1};

// not including ADC6 and ADC7 because they cannot be manipulated in software
static unsigned const char const receive_pins[4] = {PC2, PC3, PC4, PC5};
// specify which pin to use as the ADC; ADLAR set for all
// TODO: mess around with AREF settings
static unsigned const char const receive_muxes[6] = {0b01100110, 0b01100111, 0b01100010, 0b01100011, 0b01100100, 0b01100101};


void scan_pad(uint8_t send_index, uint8_t receive_index) {
    volatile unsigned char* sendport = send_ports[send_index];
    volatile unsigned char* sendddr = send_ddrs[send_index];
    unsigned char sendpin = send_pins[send_index];
    unsigned char muxval = receive_muxes[receive_index];
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
    int i;
    for (i = 0; i < 4; i++) {
        clear(DDRC, receive_pins[i]);
    }
    
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
    for (i = 0; i < 4; i++) {
        set(DDRC, receive_pins[i]);
    }
}

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Initialize various special features
    serial_init();
    
    while(1) {
        put_char(&serial_port, serial_pin_out, 'h');
        put_char(&serial_port, serial_pin_out, 'e');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'o');
        uint8_t receive_index, send_index;
        for (receive_index = 0; receive_index < 6; receive_index++) {
            for (send_index = 0; send_index < 1; send_index++) {
                put_char(&serial_port, serial_pin_out, receive_index);
                put_char(&serial_port, serial_pin_out, receive_muxes[0]);
                put_char(&serial_port, serial_pin_out, receive_muxes[(int)receive_index]);
                uint8_t is_match = receive_index == 0;
                put_char(&serial_port, serial_pin_out, is_match);
                scan_pad(send_index, receive_index);
            }
        }
        // _delay_ms(12.1);
        // _delay_ms(3.76);
    }
    
}
