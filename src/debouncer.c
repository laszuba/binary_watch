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

debounced_t debouncer(debouncer_action_t action, volatile uint16_t * timer) {
	volatile uint16_t _timer = *timer;
	debounced_t debounced = RUNNING;

	if ((_timer >= _DEBOUNCE_TICKS) && (_timer < _BUTTON_MODE_TICKS)) {
		debounced = DEBOUNCED;
	}
	else if (_timer >= _BUTTON_MODE_TICKS) {
		debounced = LONG_PRESSED;
	}

	switch (action) {
		case TST:
			break;
		case INC:
			if (debounced != LONG_PRESSED) {
				++_timer;
			}
			break;
		case RST:
			_timer = 0;
			break;
	}

	*timer = _timer;

	return debounced;
}

/**************************************************
 * Private Functions
 **************************************************/

