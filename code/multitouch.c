#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define TIMEOUT_TIME 5000
// 0 is to the right of the board (with FTDI connector down)
// send_0 = PB2
// send_1 = PA7
// send_2 = PA3
// send_3 = PA2

// 0 is at the top of the board (farthest from FTDI connector)
// receive_0 = PA0
// receive_1 = PA1
// receive_2 = PA4
// receive_3 = PA5

void scan_pad(
        volatile unsigned char* sendport,
        volatile unsigned char* sendddr,
        unsigned char sendpin,
        unsigned char muxval) {  
    // Wait for the receive pin to settle
    ADMUX = muxval;
    
    int temp_value = 100;
    while (temp_value > 10) {
        ADCSRA |= 1 << ADSC;
        while (ADCSRA & (1 << ADSC));
        temp_value = ADCL;
        temp_value += ADCH << 8;
    }

    // Make receive pins inputs
    clear(DDRA, PA1);
    clear(DDRA, PA0);
    clear(DDRA, PA5);
    clear(DDRA, PA4);
    
    // Activate the first column
    set(*sendddr, sendpin);
    set(*sendport, sendpin);
    
    // Get an analog read
    _delay_us(1);
    ADCSRA |= 1 << ADSC;
    while (ADCSRA & (1 << ADSC));
    put_char(&serial_port, serial_pin_out, ADCH);
    
    clear(*sendport, sendpin);
    clear(*sendddr, sendpin);

    // Make receive pins outputs to quickly bleed off charge.
    set(DDRA, PA1);
    set(DDRA, PA0);
    set(DDRA, PA5);
    set(DDRA, PA4);
}

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (156 kHz)
    ADCSRA = 0b10010111;
    ADCSRA = (1 << ADEN) | (1 << ADIF) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    set(ADCSRB, ADLAR);

    // Initialize various special features
    serial_init();
    
    put_ram_string(&serial_port, serial_pin_out, "Multitouch board says 'Hello, world.'");
    
    unsigned char send_pins[4] = {PA7, PA3, PB2, PA2};
    
    // Last six bits specify which pin to use as the ADC
    // {PA1, PA0, PA5, PA4}
    unsigned char receive_muxes[4] = {0b00001, 0b000000, 0b000101, 0b000100};
    
    // for some reason, all outputs only show up when shooting out of PA3
    while(1) {
        put_char(&serial_port, serial_pin_out, 'h');
        put_char(&serial_port, serial_pin_out, 'e');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'o');
        int receive_index, send_index;
        for (receive_index = 0; receive_index < 4; receive_index++) {
            for (send_index = 0; send_index < 4; send_index++) {
                if (send_index == 2)
                    scan_pad(
                        &PORTB,
                        &DDRB,
                        send_pins[send_index],
                        receive_muxes[receive_index]
                    );
                else
                    scan_pad(
                        &PORTA,
                        &DDRA,
                        send_pins[send_index],
                        receive_muxes[receive_index]
                    );
            }
        }
        _delay_ms(12.1);
        //_delay_ms(3.76);
    }
    
}
