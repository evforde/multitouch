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

#define NUM_SLAVES 2
static char slave_addresses[NUM_SLAVES] = {'a', 'b'};
static uint8_t slave_index = 0;

static unsigned char led_pins[3] = {PA3, PA2, PA1};

// ----------------------------------------------------------------------------
// LEDS
// ----------------------------------------------------------------------------

void setup_leds(void) {
    output(DDRA, PIN_LED);
    set(PORTA, PIN_LED);
    output(DDRA, led_pins[0]);
    output(DDRA, led_pins[1]);
    output(DDRA, led_pins[2]);
}

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
    TCCR1B |= (1 << CS12) | (0 << CS10); // 1 / 1024 prescaler
}

void stop_timer(void) {
    is_reading = 0;
    TCCR1B = 0;
    TCNT1 = 0;
}

ISR(TIM1_COMPA_vect) {
    set(PORTA, led_pins[0]);
    _delay_ms(10);
    clear(PORTA, led_pins[0]);
    stop_timer();
}

// ----------------------------------------------------------------------------
// SERIAL COMMUNICATION
// ----------------------------------------------------------------------------

void read_from_board(void) {
    static char chr;
    static char buffer[MAX_BUFFER] = {0};
    static int index = 0;
    get_char_interrupt(&serial_pins, MISO, &chr, &is_reading);
    put_char_no_delay(&serial_port, serial_pin_out_sft, chr);
    // Set up framing so 'h' is always first character
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
        // read from the next board
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
    serial_init(serial_pin_out_sft);
    output(DDRA, MOSI);
    setup_leds();
    setup_timer();
    
    put_char(&serial_port, serial_pin_out_sft, 'h');
    while(1) {
        // tell somebody to talk
        // stupid C problems where real indexing doesn't work...
        switch (slave_index) {
            case 0:
                put_char(&serial_port, MOSI_SFT, slave_addresses[0]);
                break;
            case 1:
                put_char(&serial_port, MOSI_SFT, slave_addresses[1]);
                break;
        }
        // show we're waiting
        set(PORTA, PIN_LED);
        start_timer();
        // wait for board reading
        while (is_reading) {
            read_from_board();
        }
        clear(PORTA, PIN_LED);
        slave_index++;
        slave_index = slave_index % NUM_SLAVES;
        _delay_ms(10);
    }
}
