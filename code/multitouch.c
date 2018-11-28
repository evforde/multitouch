#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define TIMEOUT_TIME 5000

// not including ADC6 and ADC7 because they cannot be manipulated in software
static unsigned char receive_pins[4] = {PC2, PC3, PC4, PC5};


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
    _delay_ms(1);
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
    
    static volatile unsigned char* send_ports[12] = {&PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTD, &PORTB, &PORTB, &PORTB, &PORTC, &PORTC};
    static volatile unsigned char* send_ddrs[12] = {&DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRD, &DDRB, &DDRB, &DDRB, &DDRC, &DDRC};
    static unsigned char send_pins[12] = {PD0, PD2, PD3, PD4, PD5, PD6, PD7, PB0, PB1, PB2, PC0, PC1};
    
    // specify which pin to use as the ADC
    // ADLAR set for all
    // TODO: mess around with AREF settings
    static unsigned char receive_muxes[6] = {0b01100110, 0b01100111, 0b01100010, 0b01100011, 0b01100100, 0b01100101};
    
    // for some reason, all outputs only show up when shooting out of PA3
    while(1) {
        put_char(&serial_port, serial_pin_out, 'h');
        put_char(&serial_port, serial_pin_out, 'e');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'l');
        put_char(&serial_port, serial_pin_out, 'o');
        // #TODO: try iterating through these in a different order
        // #TODO: try putting a diode between the resistor and touchpad
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[0]);
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[1]);
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[2]);
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[3]);
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[4]);
        scan_pad(send_ports[0], send_ddrs[0], send_pins[0], receive_muxes[5]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[0]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[1]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[2]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[3]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[4]);
        scan_pad(send_ports[1], send_ddrs[1], send_pins[1], receive_muxes[5]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[0]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[1]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[2]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[3]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[4]);
        scan_pad(send_ports[2], send_ddrs[2], send_pins[2], receive_muxes[5]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[0]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[1]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[2]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[3]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[4]);
        scan_pad(send_ports[3], send_ddrs[3], send_pins[3], receive_muxes[5]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[0]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[1]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[2]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[3]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[4]);
        scan_pad(send_ports[4], send_ddrs[4], send_pins[4], receive_muxes[5]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[0]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[1]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[2]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[3]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[4]);
        scan_pad(send_ports[5], send_ddrs[5], send_pins[5], receive_muxes[5]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[0]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[1]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[2]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[3]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[4]);
        scan_pad(send_ports[6], send_ddrs[6], send_pins[6], receive_muxes[5]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[0]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[1]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[2]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[3]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[4]);
        scan_pad(send_ports[7], send_ddrs[7], send_pins[7], receive_muxes[5]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[0]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[1]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[2]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[3]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[4]);
        scan_pad(send_ports[8], send_ddrs[8], send_pins[8], receive_muxes[5]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[0]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[1]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[2]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[3]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[4]);
        scan_pad(send_ports[9], send_ddrs[9], send_pins[9], receive_muxes[5]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[0]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[1]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[2]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[3]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[4]);
        scan_pad(send_ports[10], send_ddrs[10], send_pins[10], receive_muxes[5]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[0]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[1]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[2]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[3]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[4]);
        scan_pad(send_ports[11], send_ddrs[11], send_pins[11], receive_muxes[5]);
        // _delay_ms(12.1);
        // _delay_ms(3.76);
    }
    
}
