#ifndef _DEBOUNCER_H
#define _DEBOUNCER_H

/**************************************************
 * Macros
 **************************************************/

#define DEBOUNCE_TICKS 5
#define LONG_PRESS_TICKS 100

/**************************************************
 * Enums/Struts
 **************************************************/

typedef enum {
	RUN,
	STOP,
	RESET
} debouncer_state_t;

typedef enum {
	WAITING,
	DEBOUNCED,
	LONG_PRESSED
} debouncer_result_t;

typedef enum {
	SET_BTN=0,
	MODE_BTN
} button_t;

typedef struct {
	uint16_t ticks;
	debouncer_state_t state;
} debouncer_t;

/**************************************************
 * Globals
 **************************************************/

// Declare static because other files should only access the
// debouncers from public functions
static debouncer_t debouncers[];

/**************************************************
 * Public Functions
 **************************************************/

void debouncer_tick(button_t);
void debouncer_set(button_t, debouncer_state_t);
debouncer_result_t debouncer_test(button_t);

#endif /* _DEBOUNCER_H */