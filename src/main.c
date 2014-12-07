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
#include "macros.h"

// The number of timer compare events per second
#define _TICKS_SEC 128
#define _LED_TIMEOUT_TICKS 1000
#define _BUTTON_MODE_TICKS 600
#define _DEBOUNCE_TICKS 50
#define _BLINK_PERIOD 50

#define MODE !(PINC & (1 << PC0))
#define SET !(PIND & (1 << PD4))

typedef enum {
	IDLE,
	WAKE,
	SET_HOURS,
	SET_MINS
} state_t;

state_t current_state = IDLE;
state_t next_state = IDLE;

uint32_t timeout_ticks;

/**************************************************
 * Function Prototypes
 **************************************************/

void sleep_proc(void);

/**************************************************
 * Public Functions
 **************************************************/

int main(void) {

	init();

	my_time.secs = 0xFF;
	my_time.mins = 0xFF;
	my_time.hours = 0xFF;
	disp_time(ENABLE);

	// delay for one second at boot to allow the RTC to stabilize
	_delay_ms(1000);

	// Set an interesting time for testing
	my_time.secs = 0;
	my_time.mins = 13;
	my_time.hours = 7;

	// Globally enable interrupts
	sei();

	while (TRUE) {
		// Spend most of the time in sleep
		// Wake on RTC timer tick

		switch (current_state) {
			// LEDs off, waiting for button presses
			case IDLE:
				sei();
				next_state = IDLE;
				// Check for buttons
				if (SET || MODE) {
					next_state = WAKE;
				}
				break;

			// LEDs on, waiting for timeout
			case WAKE:
				next_state = WAKE;
				cli();
				disp_time(DISABLE);
				// if button is still down after timeout, enter set mode
				if (!SET && !MODE) {
					next_state = IDLE;
				}
				break;
			// Time set mode, pressing set will increment hours up to 24, then
			// roll over, pressing mode will switch to minute set mode
			case SET_HOURS:
				// Flash the hour LEDs to signify setting hours
				break;

			// Time set mode, pressing set will increment minutes up to 59, then
			// roll over, pressing mode will exit time set mode
			case SET_MINS:
				// Flash the minute LEDs to signify setting minutes
				break;

			default:
				next_state = IDLE;
				break;
		}

		current_state = next_state;

		// Put controller to sleep until next tick
		//sleep_proc();
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
	disp_time(ENABLE);
}

ISR(TIMER2_OVF_vect) 
{
	// On a timer overflow, update all the time values
	// a 'tick' happens every timer compare or 128x a second
	if (my_time.ticks == _TICKS_SEC) {
		my_time.ticks = 0;
		++my_time.secs;

		if (my_time.secs == 60) {
			my_time.secs = 0;
			++my_time.mins;

			if (my_time.mins == 60) {
				my_time.mins = 0;
				++my_time.hours;

				if (my_time.hours == 24) {
					my_time.hours = 0;
				}
			}
		}		
	}

}
