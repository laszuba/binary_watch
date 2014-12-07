#ifndef _INIT_H
#define _INIT_H

// LED PWM setting. 0-255 controls brightness of LEDs
// 0   -- minimum brightness
// 255 -- full brightness
#define _LED_BRIGHTNESS 25

typedef struct {
	uint8_t ticks;
	uint8_t secs;
	uint8_t mins;
	uint8_t hours;
} time_t;

extern time_t my_time;

void init(void);


#endif /* _INIT_H */