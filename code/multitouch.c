#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define PIN_LED PA4
//#define ADDRESS 'a'
#define ADDRESS 'b'
#define TIMEOUT_TIME 5000

static unsigned char send_pins[3] = {PB2, PA7};
static unsigned char vals[3] = {0};
// specify which pin to use as the ADC
static unsigned char receive_muxes[1] = {0b00000000, 0b00000001, 0b00000010, 0b00000011};

// TODO use MOSI interrupts so we always have updated touchpad readings
unsigned char scan_pad(
        volatile unsigned char* sendport,
        volatile unsigned char* sendddr,
        unsigned char sendpin,
        unsigned char muxval) {
    // unsigned char sendpin = send_pins[index];
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
    unsigned char is_pressed = ADCH < 6;
    put_char(&serial_port, MISO_SFT, is_pressed);
    
    clear(*sendport, sendpin);
    clear(*sendddr, sendpin);

    // Make receive pins outputs to quickly bleed off charge.
    set(DDRA, PA0);
    return is_pressed;
}

void read_touchpads(void) {
    // connect to MISO
    set(PORTA, MISO);
    set(DDRA, MISO);
    // read from the touchpads
    // Sometimes, the h goes back too fast, so delay for a little bit
    _delay_ms(10);
    put_char(&serial_port, MISO_SFT, 'h');
    put_char(&serial_port, MISO_SFT, 'e');
    put_char(&serial_port, MISO_SFT, 'y');
    uint8_t pressed = 0;
    pressed |= scan_pad(&PORTA, &DDRA, send_pins[0], receive_muxes[0]);
    /*pressed |= scan_pad(&PORTA, &DDRA, send_pins[0], receive_muxes[1]);
    pressed |= scan_pad(&PORTA, &DDRA, send_pins[0], receive_muxes[2]);
    pressed |= scan_pad(&PORTA, &DDRA, send_pins[0], receive_muxes[3]);
    pressed |= scan_pad(&PORTB, &DDRB, send_pins[1], receive_muxes[0]);
    pressed |= scan_pad(&PORTB, &DDRB, send_pins[1], receive_muxes[1]);
    pressed |= scan_pad(&PORTB, &DDRB, send_pins[1], receive_muxes[2]);
    pressed |= scan_pad(&PORTB, &DDRB, send_pins[1], receive_muxes[3]);*/
    if (pressed)
        set(PORTA, PIN_LED);
    else
        clear(PORTA, PIN_LED);
    // disconnect from MISO
    clear(DDRA, MISO);
}

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB |= (1 << ADLAR);

    output(DDRA, PIN_LED);
    // set(PORTA, PIN_LED);
    
    while(1) {
        /*static char chr;
        get_char(&serial_pins, MOSI, &chr);
        if (chr == ADDRESS || 1) {*/
            // If we hear our name, it's our turn to talk back to the master.
            read_touchpads();
        //}
    }
}
