#ifndef _INIT_H
#define _INIT_H


typedef struct {
	uint16_t ticks;
	uint8_t secs;
	uint8_t mins;
	uint8_t hours;
	uint32_t elapsed_ticks;
} time_t;


void init(void);


#endif /* _INIT_H */