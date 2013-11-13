#ifndef _TIMER_H
#define _TIMER_H

#define Timer_enable() TCCR0B = 1 << CS01	// CLK(IO) / 8
#define Timer_disable() TCCR0B = 0x00		// Disable

extern uint8_t volatile overflow, quick;
extern uint16_t volatile counter, maximum;

void Timer_init(void);
void Timer_reset(void);
void inline Timer_quick(void);
void inline Timer_normal(void);
void Timer_faster(void);
uint16_t inline Timer_speed(void);
void inline Timer_setspeed(uint16_t speed);

#endif
