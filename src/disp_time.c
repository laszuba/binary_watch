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

void disp_time(time_t * cur_time) {

	// Instead of building a struct of 0's, just pass a NULL to turn off
	// display. This is a little messy, consider an extra ON/OFF flag
	if(cur_time == NULL) {
		// LEDs off
		return;
	}

	// Map time in structure to binary time on LEDs

}
