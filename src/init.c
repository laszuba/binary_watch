/******************************************************************************
 *
 * File: init.c
 * Author: Lee Szuba
 * Created: Sept. 2014
 *
 * Usage:
 *		Initializes timers, gpio, interrupts, and other functionality
 *		for the binary watch
 *
 * See the LICENCE file included with this software for copyright information
 *
 ******************************************************************************/

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "init.h"


// set up global time fields for RTC
volatile extern time_t my_time = {
	.ticks = 0,
	.secs = 0,
	.mins = 0,
	.hours = 0,
	.elapsed_ticks = 0
}

/**************************************************
 * Function Prototypes
 **************************************************/

void timer_init(void);
void gpio_init(void);

/**************************************************
 * Public Functions
 **************************************************/

void init() {

	timer_init();
	gpio_init();

	return;
}

/**************************************************
 * Private Functions
 **************************************************/

// Set up RTC timer
void timer_init() {
	return;
}

// Set up LED outputs and button inputs
void gpio_init() {
	// Set all unused pins to inputs with pull-ups

	return;
}