#ifndef _DEBOUNCER_H
#define _DEBOUNCER_H

/**************************************************
 * Macros
 **************************************************/

#define _DEBOUNCE_TICKS 50

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
	DEBOUNCED
} debounced_t;

/**************************************************
 * Public Functions
 **************************************************/

debounced_t debouncer(debouncer_action_t);

#endif /* _DEBOUNCER_H */