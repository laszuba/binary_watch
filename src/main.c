/******************************************************************************
 *
 * File: main.c
 * Author: Lee Szuba
 * Created: Sept. 2014
 *
 * Usage:
 *		Main binary watch functionality
 *
 * See the LICENCE file included with this software for copyright information
 *
 ******************************************************************************/

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>

#include "init.h"
#include "disp_time.h"
#include "debouncer.h"
#include "macros.h"

// The number of timer compare events per second
#define _TICKS_SEC 1
//128
#define _LED_TIMEOUT_TICKS 200
#define _BLINK_PERIOD 50

#define MODE (!(PINC & (1 << PC0)))
#define SET (!(PIND & (1 << PD4)))

// State machine available states
typedef enum {
	SLEEPING,
	IDLE,
	WAKE,
	SET_HOURS,
	SET_MINS
} state_t;

volatile state_t current_state = SLEEPING;
volatile state_t next_state = SLEEPING;

volatile uint16_t mode_timer = 0;
volatile uint16_t set_timer = 0;
volatile uint16_t timeout_ticks = 0;
volatile uint16_t flash_counter;

volatile debounced_t mode_latch = RUNNING;
volatile debounced_t set_latch = RUNNING;

/**************************************************
 * Function Prototypes
 **************************************************/

void sleep_proc(void);
void startup_blink(void);

/**************************************************
 * Public Functions
 **************************************************/

int main(void) {

	init();

	// delay for at least one second at boot to allow the RTC to stabilize
	startup_blink();

	// Set an interesting time for testing
	my_time.secs  = 0;
	my_time.mins  = 0b00101010;
	my_time.hours = 0b00010101;

	// Globally enable interrupts
	sei();

	while (TRUE) {

		// Spend most of the time in sleep
		switch (current_state) {
			// LEDs off, waiting for button presses
			case SLEEPING:
				next_state = SLEEPING;
				// Go back to sleep
				//sleep_proc();
				break;

			case IDLE:
				next_state = IDLE;
				if (timeout_ticks > _LED_TIMEOUT_TICKS) {
					next_state = SLEEPING;
				}

				// Check for buttons
				if (SET || MODE) {
					next_state = WAKE;
				}
				break;

			// LEDs on, waiting for timeout
			case WAKE:
				next_state = WAKE;

				if (!SET && !MODE) {
					next_state = IDLE;
				}

				if (mode_latch == LONG_PRESSED) {
					next_state = SET_HOURS;
				}

				break;

			// Time set mode, pressing set will increment hours up to 24, then
			// roll over, pressing mode will switch to minute set mode
			case SET_HOURS:
				if ((mode_latch == LONG_PRESSED) || (mode_latch == DEBOUNCED)) {
					next_state = SET_MINS;
				}
				break;

			// Time set mode, pressing set will increment minutes up to 59, then
			// roll over, pressing mode will exit time set mode
			case SET_MINS:
				if ((mode_latch == LONG_PRESSED) || (mode_latch == DEBOUNCED)) {
					next_state = WAKE;
				}
				break;

			default:
				next_state = IDLE;
				break;
		}

		if (current_state == SLEEPING) {
			disp_off();
			disp_time(DISABLE);
			disable_pwm();
			enable_pcint();
		}
		else {
			if (((current_state == SET_MINS) ||
				(current_state == SET_HOURS)) &&
				(flash_counter & 0x04)) {
				disp_off();
			}
			else {
				disp_on();
			}
			enable_pwm();
			disable_pcint();
		}

		if (next_state != current_state) {
			mode_latch = CLEAR;
			set_latch = CLEAR;
			current_state = next_state;
		}
	}

	return 0;
}

/**************************************************
 * Private Functions
 **************************************************/

// IMPORTANT: if interrupts are not enabled, the processor will never wake
// from sleep
void sleep_proc() {
	set_sleep_mode(SLEEP_MODE_IDLE);

	sleep_enable();
	sleep_mode();
 
	// Continue here on wake
	sleep_disable();

	return;
}

void startup_blink() {
	disp_on();

	for (uint8_t i = 0; i < 2; ++i) {
		my_time.secs = 0xFF;
		my_time.mins = 0xFF;
		my_time.hours = 0xFF;
		disp_time(ENABLE);

		_delay_ms(500);

		my_time.secs = 0x00;
		my_time.mins = 0x00;
		my_time.hours = 0x00;
		disp_time(ENABLE);

		_delay_ms(500);
	}

	return;
}

/**************************************************
 * ISR
 **************************************************/

// On compare set the LEDs off
ISR(TIMER0_COMPA_vect) {
	disp_time(DISABLE);
}

// On overflow, turn the LEDs back on
ISR(TIMER0_OVF_vect) {
	my_time.hours = current_state;
	disp_time(ENABLE);

	if (current_state == IDLE) ++timeout_ticks;
	else timeout_ticks = 0;

	++flash_counter;

	if (MODE) {
		// While the button is pressed, increment the debounce timer
		debouncer(INC, &mode_timer);
	}
	else {
		if (mode_latch == CLEAR) {
			// If the main loop has cleared the mode flag
			// latch the output of the debouncer
			mode_latch = debouncer(RST, &mode_timer);
		}
	}

	if (SET) {
		// While the button is pressed, increment the debounce timer
		debouncer(INC, &set_timer);
	}
	else {
		if (set_latch == CLEAR) {
			// If the main loop has cleared the mode flag
			// latch the output of the debouncer
			set_latch = debouncer(RST, &set_timer);
		}
	}
}

ISR(TIMER2_OVF_vect) {
	// On a timer overflow, update all the time values
	// a 'tick' happens every timer compare or 128x a second
	++my_time.ticks;

	if (my_time.ticks >= _TICKS_SEC) {
		my_time.ticks = 0;
		++my_time.secs;

		if (my_time.secs >= 59) {
			my_time.secs = 0;
			++my_time.mins;

			if (my_time.mins >= 59) {
				my_time.mins = 0;
				++my_time.hours;

				if (my_time.hours >= 23) {
					my_time.hours = 0;
				}
			}
		}		
	}

}

ISR(PCINT1_vect) {
	next_state = WAKE;
}

ISR(PCINT2_vect, ISR_ALIASOF(PCINT1_vect));

