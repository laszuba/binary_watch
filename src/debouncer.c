/******************************************************************************
 *
 * File: debouncer.c
 * Author: Lee Szuba
 * Created: Dec. 2014
 *
 * Usage:
 *		Timer used to debounce a pushbutton
 *
 * See the LICENCE file included with this software for copyright information
 *
 ******************************************************************************/

#include <avr/interrupt.h>

#include "debouncer.h"
#include "macros.h"


// Create a global array of debouncers, one per button
static debouncer_t debouncers[] = {
	{0,STOP},
	{0,STOP}
};

/**************************************************
 * Local Function Prototypes
 **************************************************/

/**************************************************
 * Public Functions
 **************************************************/

// Debouncer tick should be called periodically by the timer ISR
void debouncer_tick(button_t button) {
	debouncer_t * my_debouncer = &debouncers[button];

	// Increment the number of ticks only if the debouncer is running
	if (my_debouncer->state == RUN) {
		my_debouncer->ticks++;
	}

	// To avoid rollover, stop after reaching the 'longest' state
	if (my_debouncer->ticks > LONG_PRESS_TICKS) {
		my_debouncer->state = STOP;
	}
	return;
}

void debouncer_set(button_t button, debouncer_state_t cmd) {
	debouncer_t * my_debouncer = &debouncers[button];

	switch (cmd) {
		case RUN:
			my_debouncer->state = RUN;
			break;
		case STOP:
			my_debouncer->state = STOP;
			break;
		case RESET:
			my_debouncer->ticks = 0;
			break;
	}
	return;
}

debouncer_result_t debouncer_test(button_t button) {
	debouncer_t * my_debouncer = &debouncers[button];
	debouncer_result_t result = WAITING;

	if (my_debouncer->ticks >= LONG_PRESS_TICKS) {
		result = LONG_PRESSED;
	}
	else if (my_debouncer->ticks >= DEBOUNCE_TICKS) {
		result = DEBOUNCED;
	}

	return result;
}

/**************************************************
 * Local Functions
 **************************************************/

