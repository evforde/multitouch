#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"
#include "multitouch.h"

// TODO
#define ADDRESS '\xe0'
// TODO

#define PIN_LED PA4

#define MISO PA5
#define MISO_SFT (1 << MISO)
#define MOSI PA6
#define MOSI_SFT (1 << MOSI)

static unsigned char send_pins[2] = {PA7, PB2};
// specify which pin to use as the ADC
static unsigned char receive_muxes[4] = {0b00000000, 0b00000001, 0b00000010, 0b00000011};

static char last_pressed_pads = 0;
static char needs_update = 0;

// ----------------------------------------------------------------------------
// READ TOUCHPADS
// ----------------------------------------------------------------------------

void rx_input(void) {
    clear(DDRA, PA0); clear(DDRA, PA1); clear(DDRA, PA2); clear(DDRA, PA3);
}

void rx_output(void) {
    set(DDRA, PA0); set(DDRA, PA1); set(DDRA, PA2); set(DDRA, PA3);
    clear(PORTA, PA0); clear(PORTA, PA1); clear(PORTA, PA2); clear(PORTA, PA3);
}

void read_touchpads(void) {
    // pressed_pads is an eight-bit representation of which pads are touched
    // Trying to scan through them in a different order to minimize leakage
    char pressed_pads = 0;
    pressed_pads |= scan_pad(&PORTA, &DDRA, send_pins[0], &rx_input, &rx_output, receive_muxes[1], 21) << 1;
    pressed_pads |= scan_pad(&PORTA, &DDRA, send_pins[0], &rx_input, &rx_output, receive_muxes[3], 21) << 3;
    pressed_pads |= scan_pad(&PORTA, &DDRA, send_pins[0], &rx_input, &rx_output, receive_muxes[0], 21) << 0;
    pressed_pads |= scan_pad(&PORTA, &DDRA, send_pins[0], &rx_input, &rx_output, receive_muxes[2], 21) << 2;
    pressed_pads |= scan_pad(&PORTB, &DDRB, send_pins[1], &rx_input, &rx_output, receive_muxes[1], 21) << 5;
    pressed_pads |= scan_pad(&PORTB, &DDRB, send_pins[1], &rx_input, &rx_output, receive_muxes[3], 21) << 7;
    pressed_pads |= scan_pad(&PORTB, &DDRB, send_pins[1], &rx_input, &rx_output, receive_muxes[0], 21) << 4;
    pressed_pads |= scan_pad(&PORTB, &DDRB, send_pins[1], &rx_input, &rx_output, receive_muxes[2], 21) << 6;
    last_pressed_pads = pressed_pads;
    if (pressed_pads)
        set(PORTA, PIN_LED);
    else
        clear(PORTA, PIN_LED);
    needs_update = 0;
}

// ----------------------------------------------------------------------------
// SERIAL INTERRUPT
// ----------------------------------------------------------------------------

ISR(PCINT0_vect) {
    static char chr;
    // Ensure we always serve new data - if there wasn't time to refresh
    // yet, don't return anything
    if (needs_update)
        return;
    get_char_timeout(&PINA, MOSI, &chr, 1e6);
    if (chr == ADDRESS) {
        // give the master board a bit of time
        set(PORTA, PIN_LED);
        _delay_ms(10);
        clear(PORTA, PIN_LED);
        _delay_ms(10);
        // connect to MISO
        set(PORTA, MISO);
        set(DDRA, MISO);
        // output the state of the touchpads
        put_char_no_delay(&PORTA, MISO_SFT, last_pressed_pads);
        needs_update = 1;
        // disconnect from MISO
        clear(DDRA, MISO);
    }
}

// ----------------------------------------------------------------------------
// MAIN CODE
// ----------------------------------------------------------------------------

int main(void)
{
    // set clock divider to /1
    CLKPR = (1 << CLKPCE);
    CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);

    // ADC enabled, flag cleared, prescalar set to 128 (125 kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRB |= (1 << ADLAR);

    // Set up pin change interrupt
    GIMSK = (1 << PCIE0);
    PCMSK0 = (1 << MOSI);
    sei();

    output(DDRA, PIN_LED);
    
    while(1) {
        read_touchpads();
    }
}
