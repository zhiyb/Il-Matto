#ifndef _SOUND_H
#define _SOUND_H

#define Sound_DDR	DDRB
#define Sound_PORT	PORTB

#define Sound_IO	(1 << 7)

void Sound_init(void);
void Sound(uint8_t freq, uint8_t ms);

#endif
