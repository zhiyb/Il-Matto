#ifndef SOUND_H
#define SOUND_H

#define SOUND_DDR	DDRB
#define SOUND_PORT	PORTB
#define SOUND_PIN	PINB
#define SOUND_P		_BV(7)
#define SOUND_LENGTH	50

#define SOUND_SELECT	352
#define SOUND_CONF	1047
#define SOUND_FAILED	262
#define SOUND_CANCEL	1047
#define SOUND_EDGE	523
#define SOUND_TOUCH	1047//587

#include <avr/io.h>
#include "timer.h"

static inline void sound_init(void)
{
	SOUND_DDR |= SOUND_P;
	SOUND_PORT &= ~SOUND_P;
	timer1_init();
}

static inline void sound_freq(uint16_t freq)
{
	freq = F_CPU / 8 / 2 / freq;
	timer1_disable();
	OCR1AH = freq / 0x0100;
	OCR1AL = freq % 0x0100;
	timer1_enable();
}

#endif
