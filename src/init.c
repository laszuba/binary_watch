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
#include "macros.h"


// set up global time fields for RTC
time_t my_time = {
	.ticks = 0,
	.secs = 0,
	.mins = 0,
	.hours = 0,
};

/**************************************************
 * Local Function Prototypes
 **************************************************/

void timer_init(void);
void gpio_init(void);

/**************************************************
 * Public Functions
 **************************************************/

void init() {

	// Disable the analog comparator to save some power
	ACSR |= (1 << ACD);

	// Disable the ADC
	//TODO
	// Shut off unused peripherals
	// ADC must be disabled before turning off clock
	PRR |= (1 << PRTWI) | (0 << PRTIM2) | (0 << PRTIM0) |
	       (1 << PRTIM1) | (1 << PRSPI) | (1 << PRUSART0) |
	       (0 << PRADC);

	timer_init();
	gpio_init();

	return;
}

/**************************************************
 * Private Functions
 **************************************************/

// Set up RTC timer
void timer_init() {

	// Set timer 0 prescaler to /8
	TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
	OCR0A = _LED_BRIGHTNESS;

	// Interrupt on compare A and overflow
	TIMSK0 |= (0 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);

	// Set timer 2 prescaler to /1 (128 Hz)
	//TCCR2B |= (0 << CS22) | (0 << CS21) | (1 << CS20);

	// Set timer 2 prescaler to /128 (1 Hz)
	TCCR2B |= (1 << CS22) | (0 << CS21) | (1 << CS20);

	// Interrupt on overflow
	TIMSK2 |= (0 << OCIE2B) | (0 << OCIE2A) | (1 << TOIE2);

	// Enable the external crystal input to use timer 2 as an RTC
	ASSR |= (1 << AS2);

	return;
}

// Set up LED outputs and button inputs
void gpio_init() {
	// Set all unused pins to inputs with pull-ups

	// Set LED pins to output
	DDRB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
	DDRC |= (1 << PC2) | (1 << PC1);
	DDRD |= (1 << PD7) | (1 << PD6) | (1 << PD5) | (1 << PD3) | (1 << PD2) | (1 << PD0);

	// Make sure all the LEDs are off
	PORTB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
	PORTC |= (1 << PC2) | (1 << PC1);
	PORTD |= (1 << PD7) | (1 << PD6) | (1 << PD5) | (1 << PD3) | (1 << PD2) | (1 << PD0);

	// Enable pin change interrupts on two ports buttons are on
	PCICR |= (1 << PCIE2) | (1 << PCIE1) | (0 << PCIE0);
	// And configure the masks
	PCMSK2 |= (1 << PCINT20);
	PCMSK1 |= (1 << PCINT8);

	return;
}