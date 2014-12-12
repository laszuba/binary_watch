#ifndef _DEBOUNCER_H
#define _DEBOUNCER_H

/**************************************************
 * Macros
 **************************************************/

#define _DEBOUNCE_TICKS 20
#define _BUTTON_MODE_TICKS 300

/**************************************************
 * Enums/Struts
 **************************************************/

typedef enum {
	TST,
	INC,
	RST
} debouncer_action_t;

typedef enum {
	RUNNING,
	DEBOUNCED,
	LONG_PRESSED,
	CLEAR
} debounced_t;

/**************************************************
 * Public Functions
 **************************************************/

debounced_t debouncer(debouncer_action_t, volatile uint16_t *);

#endif /* _DEBOUNCER_H */