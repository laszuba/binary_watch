#ifndef _MAIN_H
#define _MAIN_H

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <util/delay.h>

#include "init.h"
#include "disp_time.h"
#include "debouncer.h"
#include "macros.h"

/**************************************************
 * Macros
 **************************************************/

// The number of timer compare events per second
#define _TICKS_SEC 60

#define _LED_TIMEOUT_TICKS 300
#define _BLINK_PERIOD 10
#define _BLINK_DUTY 50
#define _BLINK_ON_TIME ((_BLINK_PERIOD * _BLINK_DUTY) / 100)
#define _QUICK_INC_RATE 5

#define MODE (!(PINC & (1 << PC0)))
#define SET (!(PIND & (1 << PD4)))

/**************************************************
 * Enums/Struts
 **************************************************/

// State machine available states
typedef enum {
	SLEEPING,
	IDLE,
	WAKE,
	READY_TO_SET_HOURS,
	SET_HOURS,
	QUICK_HOURS_INC,
	READY_TO_SET_MINS,
	SET_MINS,
	QUICK_MINS_INC
} state_t;

/**************************************************
 * Globals
 **************************************************/

volatile state_t current_state = SLEEPING;
volatile state_t next_state = SLEEPING;

volatile uint16_t timeout_ticks = 0;
volatile uint16_t flash_counter;
volatile uint16_t time_set_counter = 0;
volatile uint16_t quick_inc_timer = 0;
volatile uint8_t quick_inc = FALSE;

#endif /*_MAIN_H */