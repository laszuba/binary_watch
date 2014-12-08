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

/**************************************************
 * Local Function Prototypes
 **************************************************/

/**************************************************
 * Public Functions
 **************************************************/

debounced_t debouncer(debouncer_action_t action) {
	volatile static uint16_t timer;
	debounced_t debounced = RUNNING;

	if (timer >= _DEBOUNCE_TICKS) {
		debounced = DEBOUNCED;
	}

	switch (action) {
		case TST:
			break;
		case INC:
			if (debounced == RUNNING) {
				++timer;
			}
			break;
		case RST:
			timer = 0;
			break;
	}

	return debounced;
}

/**************************************************
 * Private Functions
 **************************************************/

