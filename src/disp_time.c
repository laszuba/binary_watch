/******************************************************************************
 *
 * File: disp_time.c
 * Author: Lee Szuba
 * Created: Sept. 2014
 *
 * Usage:
 *		Contains functions to display time on binary watch LEDs
 *
 * See the LICENCE file included with this software for copyright information
 *
 ******************************************************************************/

#include <avr/io.h>

#include "disp_time.h"
#include "init.h"
#include "macros.h"


// LEDS
// 
// Minutes:
//	M1 -> bit0 -> PD5
//	M2 -> bit1 -> PD6
//	M3 -> bit2 -> PD7
//	M4 -> bit3 -> PB0
//	M5 -> bit4 -> PB1
//	M6 -> bit5 -> PB2
//
// Hours:
//	H1 -> bit0 -> PC1
//	H2 -> bit1 -> PC2
//	H3 -> bit2 -> PD0
//	H4 -> bit3 -> PD2
//	H5 -> bit4 -> PD3
//
// All are active low

volatile static enabled_t disp_enable = DISABLE;

/**************************************************
 * Public Functions
 **************************************************/

void disp_time(uint8_t pwm_state) {
	uint8_t hours_leds = 0;
	uint8_t mins_leds = 0;

	if ((disp_enable == ENABLE) && (pwm_state == ENABLE)) {
		hours_leds = my_time.hours;
		mins_leds = my_time.mins;
	}

	// Make sure all the LEDs are off
	PORTB |= (1 << PB2) | (1 << PB1) | (1 << PB0);
	PORTC |= (1 << PC2) | (1 << PC1);
	PORTD |= (1 << PD7) | (1 << PD6) | (1 << PD5) | (1 << PD3) | (1 << PD2) | (1 << PD0);

	// Map time in structure to binary time on LEDs
	PORTC &=  ~((0b00000011 & hours_leds) << 1);
	PORTD &= ~(((0b00000100 & hours_leds) >> 2)
	         | ((0b00011000 & hours_leds) >> 1)
	         | ((0b00000111 & mins_leds ) << 5));
	PORTB &=  ~((0b00111000 & mins_leds ) >> 3);

	return;
}

void disp_off() {
	disp_enable = DISABLE;
	return;
}
void disp_on() {
	disp_enable = ENABLE;
	return;
}
