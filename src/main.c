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
#define _TICKS_SEC 1
//128
#define _LED_TIMEOUT_TICKS 200
#define _BUTTON_MODE_TICKS 600
#define _DEBOUNCE_TICKS 50
#define _BLINK_PERIOD 50

#define MODE !(PINC & (1 << PC0))
#define SET !(PIND & (1 << PD4))

typedef enum {
	SLEEPING,
	IDLE,
	WAKE,
	SET_HOURS,
	SET_MINS
} state_t;

volatile state_t current_state = SLEEPING;
volatile state_t next_state = SLEEPING;

volatile uint16_t timeout_ticks;
volatile uint8_t disp_enable;

/**************************************************
 * Function Prototypes
 **************************************************/

void sleep_proc(void);
void startup_blink(void);
void enable_pcint(void);
void disable_pcint(void);

/**************************************************
 * Public Functions
 **************************************************/

int main(void) {

	init();

	// delay for at least one second at boot to allow the RTC to stabilize
	startup_blink();

	// Set an interesting time for testing
	my_time.secs = 0;
	my_time.mins = 13;
	my_time.hours = 7;

	// Globally enable interrupts
	sei();

	while (TRUE) {

		// Spend most of the time in sleep
		disable_pcint();
		switch (current_state) {
			// LEDs off, waiting for button presses
			case SLEEPING:
				enable_pcint();
				next_state = SLEEPING;
				disp_enable = DISABLE;
				// Go back to sleep
				_delay_ms(100); // fake sleep
				//sleep_proc();
				break;

			case IDLE:
				next_state = IDLE;
				disp_enable = ENABLE;
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
				disp_enable = ENABLE;

				if (!SET && !MODE) {
					next_state = IDLE;
				}
				if ((SET || MODE) && (timeout_ticks > _BUTTON_MODE_TICKS)) {
					next_state = SET_HOURS;
				}
				break;

			// Time set mode, pressing set will increment hours up to 24, then
			// roll over, pressing mode will switch to minute set mode
			case SET_HOURS:
				disp_enable = ENABLE;

				if (SET) {
					my_time.hours++;
				}

				if (MODE) {
					next_state = SET_MINS;
				}
				// Flash the hour LEDs to signify setting hours
				break;

			// Time set mode, pressing set will increment minutes up to 59, then
			// roll over, pressing mode will exit time set mode
			case SET_MINS:
				// Flash the minute LEDs to signify setting minutes
				disp_enable = ENABLE;

				if (SET) {
					my_time.mins++;
				}

				break;

			default:
				next_state = IDLE;
				break;
		}

		current_state = next_state;

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

	my_time.secs = 0xFF;
	my_time.mins = 0xFF;
	my_time.hours = 0xFF;
	disp_time(ENABLE);

	_delay_ms(500);

	return;
}

void disable_pcint() {
	PCICR &= ~((1 << PCIE2) | (1 << PCIE1));
}

void enable_pcint() {
	PCICR |= (1 << PCIE2) | (1 << PCIE1);
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
	disp_time(disp_enable);
	++timeout_ticks;
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
	timeout_ticks = 0;
}

ISR(PCINT2_vect, ISR_ALIASOF(PCINT1_vect));