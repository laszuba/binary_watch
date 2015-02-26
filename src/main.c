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

#include "main.h"

uint8_t last_set_state = FALSE;

/**************************************************
 * Function Prototypes
 **************************************************/

void state_machine(void);
void flash_mins(void);
void flash_hours(void);
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
		state_machine();
	}

	return 0;
}

/**************************************************
 * Private Functions
 **************************************************/

void state_machine() {
	// Spend most of the time in sleep
	switch (current_state) {
		// LEDs off, waiting for button presses
		case SLEEPING:
			next_state = SLEEPING;
			disp_off();
			disp_time(DISABLE);
			disable_pwm();
			enable_pcint();

			//TODO: Go back to sleep
			//sleep_proc();
			break;

		case IDLE:
			next_state = IDLE;

			if (timeout_ticks > _LED_TIMEOUT_TICKS) {
				next_state = SLEEPING;
			}

			if (SET || MODE) next_state = WAKE;

			break;

		// LEDs on, waiting for timeout
		case WAKE:
			next_state = WAKE;
			disable_pcint();
			disp_on();
			enable_pwm();

			if (!SET && !MODE) {
				next_state = IDLE;
			}

			if ((debouncer_test(SET_BTN) == LONG_PRESSED) &&
			    (debouncer_test(MODE_BTN) == LONG_PRESSED)) {
				next_state = READY_TO_SET_HOURS;
			}
			break;

		case READY_TO_SET_HOURS:
			next_state = READY_TO_SET_HOURS;
			last_set_state = FALSE;

			flash_mins();
			flash_hours();

			if (!MODE && !SET) next_state = SET_HOURS;

			break;

		// Time set mode, pressing set will increment hours up to 24, then
		// roll over, pressing mode will switch to minute set mode
		case SET_HOURS:
			next_state = SET_HOURS;

			flash_mins();

			if (debouncer_test(MODE_BTN) == DEBOUNCED) {
				next_state = READY_TO_SET_MINS;
			}

			if (debouncer_test(SET_BTN) == DEBOUNCED) {
				last_set_state = TRUE;
			}
			else if (debouncer_test(SET_BTN) == LONG_PRESSED) {
				next_state = QUICK_HOURS_INC;
			}

			if ((last_set_state == TRUE) && !SET) {
				last_set_state = FALSE;
				++my_time.hours;
			}
			break;

		case QUICK_HOURS_INC:
			next_state = QUICK_HOURS_INC;

			flash_mins();
			
			if (!SET) next_state = SET_HOURS;

			if (quick_inc) {
				quick_inc = FALSE;
				++my_time.hours;
			}

			break;

		case READY_TO_SET_MINS:
			next_state = READY_TO_SET_MINS;
			last_set_state = FALSE;

			flash_mins();
			flash_hours();

			if (!MODE) next_state = SET_MINS;

			break;

		// Time set mode, pressing set will increment minutes up to 59, then
		// roll over, pressing mode will exit time set mode
		case SET_MINS:
			next_state = SET_MINS;

			flash_hours();

			if (debouncer_test(MODE_BTN) == DEBOUNCED) {
				next_state = WAKE;
			}

			if (debouncer_test(SET_BTN) == DEBOUNCED) {
				last_set_state = TRUE;
			}
			else if (debouncer_test(SET_BTN) == LONG_PRESSED) {
				next_state = QUICK_MINS_INC;
			}

			if ((last_set_state == TRUE) && !SET) {
				last_set_state = FALSE;
				++my_time.mins;
			}
			break;

		case QUICK_MINS_INC:
			next_state = QUICK_MINS_INC;

			flash_hours();

			if (!SET) next_state = SET_MINS;

			if (quick_inc) {
				quick_inc = FALSE;
				++my_time.mins;
			}

			break;
		default:
			next_state = IDLE;
			break;
	}

	if (current_state != SLEEPING) {
		test_buttons();
	}

	if (next_state != current_state) {
		current_state = next_state;
	}
}

void test_buttons() {
	if (!SET) {
		debouncer_set(SET_BTN, STOP);
		debouncer_set(SET_BTN, RESET);
	}
	else {
		debouncer_set(SET_BTN, RUN);
	}
	if (!MODE) {
		debouncer_set(MODE_BTN, STOP);
		debouncer_set(MODE_BTN, RESET);
	}
	else {
		debouncer_set(MODE_BTN, RUN);
	}
}

void flash_mins() {
	if (flash_counter > _BLINK_ON_TIME) disp_on();
	else mins_off();

	return;
}

void flash_hours() {
	if (flash_counter > _BLINK_ON_TIME) disp_on();
	else hours_off();

	return;
}

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

// Timer 0 handles the PWM for the LEDs, 

// On compare set the LEDs off
ISR(TIMER0_COMPA_vect) {
	disp_time(DISABLE);
}

ISR(TIMER0_OVF_vect) {
	// On overflow, turn the LEDs back on
	//TODO: remove after debugging
	my_time.hours = current_state;
	disp_time(ENABLE);

	if (current_state == IDLE) ++timeout_ticks;
	else timeout_ticks = 0;

	if (flash_counter < _BLINK_PERIOD) ++flash_counter;
	else flash_counter = 0;

	if (quick_inc_timer < _QUICK_INC_RATE) {
		++quick_inc_timer;
	}
	else {
		quick_inc_timer = 0;
		quick_inc = TRUE;
	}

	debouncer_tick(MODE_BTN);
	debouncer_tick(SET_BTN);
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

