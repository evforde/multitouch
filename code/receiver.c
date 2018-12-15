#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include "serial.h"
#include "macros.h"

#define serial_pin_out PD1
#define serial_pin_out_sft (1 << serial_pin_out)
#define PIN_LED PB5
#define ADDRESS 0
#define TIMEOUT_TIME 5000

#define MISO PB4
#define MISO_SFT (1 << MISO)
#define MOSI PB3
#define MOSI_SFT (1 << MOSI)

#define NUM_STRINGS 3
#define NUM_FRETS 2
#define MAX_BUFFER 3 + NUM_STRINGS
static unsigned char string_values[NUM_STRINGS] = {0};

#define NUM_SLAVES 2
static char slave_addresses[NUM_SLAVES] = {'a', 'b'};
static uint8_t slave_index = 0;


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
    // TODO decrease prescaler
    TCCR1B |= (1 << CS12) | (0 << CS10); // 1 / 512 prescaler
}

void stop_timer(void) {
    is_reading = 0;
    TCCR1B = 0;
    TCNT1 = 0;
}

ISR(TIM1_COMPA_vect) {
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
    // TODO don't need to put out over serial
    // put_char_no_delay(&serial_port, serial_pin_out_sft, chr);
    // Set up framing so 'h' is always first character
    if (chr == 'h' || index == MAX_BUFFER) {
        index = 0;
    }
    buffer[index++] = chr;
    // Check for start pattern and a full buffer
    if (buffer[0] == 'h' && buffer[1] == 'e' && buffer[2] == 'y' &&
            index == MAX_BUFFER) {
        // TODO we never check that the address is the correct slave here
        int string_index = 0;
        for (string_index = 0; string_index < NUM_STRINGS; string_index++) {
            // TODO update for multiple frets per slave board
            if (buffer[3 + string_index])
                set(string_values[string_index], slave_index);
            else
                clear(string_values[string_index], slave_index);
        }
        stop_timer();
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

    // Initialize various special features
    serial_init();
    // setup_timer();
    
    put_char(&PORTD, serial_pin_out_sft, 'h');
    while(1) {
        if (bit_test(PINB, MISO))
            set(PORTD, serial_pin_out);
        else
            clear(PORTD, serial_pin_out);
        continue;
        // tell somebody to talk
        // stupid C problems where real indexing doesn't work...
        switch (slave_index) {
            case 0:
                put_char(&PORTD, MOSI_SFT, slave_addresses[0]);
                break;
            case 1:
                put_char(&PORTD, MOSI_SFT, slave_addresses[1]);
                break;
        }
        // show we're waiting
        set(PORTB, PIN_LED);
        start_timer();
        // wait for board reading
        while (is_reading) {
            read_from_board();
        }
        clear(PORTB, PIN_LED);
        slave_index++;
        slave_index = slave_index % NUM_SLAVES;
        _delay_ms(10);
    }
}
