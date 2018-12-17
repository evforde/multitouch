#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"
//#include "multitouch.h"

#define serial_pin_out PD1
#define serial_pin_out_sft (1 << serial_pin_out)

#define PIN_LED PB5

#define MISO PB4
#define MISO_SFT (1 << MISO)
#define MOSI PB3
#define MOSI_SFT (1 << MOSI)

#define NUM_STRINGS 4
#define NUM_FRETS 2
#define NUM_READINGS NUM_STRINGS * NUM_FRETS
#define MAX_BUFFER 2
static unsigned char string_values[NUM_STRINGS] = {0};

#define NUM_SLAVES 3
static char slave_addresses[NUM_SLAVES] = {'\xe0', '\xe1', '\xe2'};
static uint8_t slave_index = 0;

static unsigned char send_pins[2] = {PC0, PC1};
// specify which pin to use as the ADC
static unsigned char receive_muxes[2] = {0b00100110, 0b00100111};

// ----------------------------------------------------------------------------
// TIMER
// ----------------------------------------------------------------------------

static unsigned char is_reading = 0;
void setup_timer(void) {
    // 16-bit counter with top=0xffff
    TIMSK1 |= (1 << OCIE1A); // Set the interrupt handler
    sei();
}

void start_timer(void) {
    is_reading = 1;
    // TCCR1B |= (1 << CS11) | (1 << CS10); // 1 / 64 prescaler
    TCCR1B |= (1 << CS12); // 1 / 256 prescaler
}

void stop_timer(void) {
    is_reading = 0;
    TCCR1B = 0;
    TCNT1 = 0;
}

ISR(TIMER1_COMPA_vect) {
    stop_timer();
}

// ----------------------------------------------------------------------------
// SERIAL COMMUNICATION
// ----------------------------------------------------------------------------

void serial_init() {
    set(PORTD, serial_pin_out);
    output(DDRD, serial_pin_out);
    output(DDRB, MOSI);
}

void read_from_board(void) {
    static char chr;
    static char buffer[MAX_BUFFER] = {0};
    static int index = 0;
    get_char_interrupt(&PINB, MISO, &chr, &is_reading);
    // put_char_no_delay(&PORTD, serial_pin_out_sft, chr);
    // Set up framing so 'h' is always first character
    if (chr == 'h' || index == MAX_BUFFER) {
        index = 0;
    }
    buffer[index++] = chr;
    // Check for start pattern and a full buffer
    if (buffer[0] == 'h' && index == MAX_BUFFER) {
        uint8_t readings = buffer[1];
        uint8_t reading_index = 0;
        for (reading_index = 0; reading_index < NUM_READINGS; reading_index++) {
            uint8_t string_index = reading_index % NUM_STRINGS;
            uint8_t fret_index = slave_index * NUM_FRETS + (reading_index / NUM_STRINGS);
            if bit_test(readings, reading_index)
                set(string_values[string_index], fret_index);
            else
                clear(string_values[string_index], fret_index);
        }
        stop_timer();
    }
}

void get_notes(void) {
    // show we're waiting
    for (slave_index = 0; slave_index < 3; slave_index++) {
        start_timer();
        set(PORTB, PIN_LED);
        // tell somebody to talk
        // stupid problem where real indexing doesn't work...
        switch (slave_index) {
            case 0: put_char(&PORTB, MOSI_SFT, slave_addresses[0]); break;
            case 1: put_char(&PORTB, MOSI_SFT, slave_addresses[1]); break;
            case 2: put_char(&PORTB, MOSI_SFT, slave_addresses[2]); break;
        }
        // wait for board reading
        while (is_reading)
            read_from_board();
        clear(PORTB, PIN_LED);
        _delay_ms(10);
    }
    // Send the state of all strings to the computer
    put_char(&PORTD, serial_pin_out_sft, 'h');
    put_char(&PORTD, serial_pin_out_sft, string_values[0]);
    put_char(&PORTD, serial_pin_out_sft, string_values[1]);
    put_char(&PORTD, serial_pin_out_sft, string_values[2]);
    put_char_no_delay(&PORTD, serial_pin_out_sft, string_values[3]);
}

// ----------------------------------------------------------------------------
// TOUCH SENSING
// ----------------------------------------------------------------------------

void noop(void) {}

void read_strum_touchpads(void) {
    // strummed is an eight-bit representation of which pads are touched
    uint8_t strummed = 0;
    strummed |= scan_pad(&PORTC, &DDRC, send_pins[0], &noop, &noop, receive_muxes[0], 26) << 3;
    strummed |= scan_pad(&PORTC, &DDRC, send_pins[0], &noop, &noop, receive_muxes[1], 24) << 2;
    strummed |= scan_pad(&PORTC, &DDRC, send_pins[1], &noop, &noop, receive_muxes[0], 28) << 1;
    strummed |= scan_pad(&PORTC, &DDRC, send_pins[1], &noop, &noop, receive_muxes[1], 26) << 0;
    // If any string is hit, ask the other boards what notes are held down
    if (strummed)
        get_notes();
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

    // Initialize various special features
    serial_init();
    setup_timer();
    output(DDRB, PIN_LED);
    
    put_char(&PORTD, serial_pin_out_sft, 'h');
    put_char(&PORTD, serial_pin_out_sft, 'i');
    while(1) {
        read_strum_touchpads();
    }
}
