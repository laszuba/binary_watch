#ifndef _MACROS_H
#define _MACROS_H

/**************************************************
 * Macros
 **************************************************/

#define NULL 0

#define TRUE 1
#define FALSE 0

#define disable_pcint() (PCICR &= ~((1 << PCIE2) | (1 << PCIE1)))
#define enable_pcint() (PCICR |= (1 << PCIE2) | (1 << PCIE1))

#define disable_pwm() PRR |= (1 << PRTIM0)
#define enable_pwm() PRR &= ~(1 << PRTIM0)

/**************************************************
 * Enums
 **************************************************/

typedef enum {
	DISABLE = 0,
	ENABLE = 1
} enabled_t;

#endif /* _MACROS_H */