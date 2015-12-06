// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <macros.h>
#include "rgbled.h"

uint32_t rgbLED[RGBLED_NUM] = {
	0x00ff0000, 0x0000ff00, 0x000000ff,
	0x00ffff00, 0x00ff00ff, 0x0000ffff,
	0x00ffffff,
	0x00000000,
};

#define DDR	CONCAT_E(DDR, RGBLED_PORT)
#define PORT	CONCAT_E(PORT, RGBLED_PORT)
#define DIN	_BV(RGBLED_DIN)

static inline void reset()
{
	PORT &= ~DIN;
	uint8_t cnt = 50 * 3;	// > 50us
	while (cnt--) {
		while (!(TIFR2 & _BV(OCF2A)));
		TIFR2 |= _BV(OCF2A);
	}
}

static void send(uint8_t data)
{
	uint8_t cnt = 8;
	do {
		while (!(TIFR2 & _BV(OCF2A)));
		PORT |= DIN;
		TIFR2 |= _BV(OCF2A);
		if (!(data & 0x80))
			PORT &= ~DIN;
		while (!(TIFR2 & _BV(OCF2A)));
		TIFR2 |= _BV(OCF2A);
		while (!(TIFR2 & _BV(OCF2A)));
		PORT &= ~DIN;
		TIFR2 |= _BV(OCF2A);
		data <<= 1;
	} while (--cnt);
}

void rgbLED_init()
{
	DDR |= DIN;
	PORT &= ~DIN;

	// Use timer 2 for transmission timing
	TCCR2A = _BV(WGM21);
	TCCR2B = 0;
	TCNT2 = 0;
	OCR2A = F_CPU * 4UL / 10000000UL;	// 0.4us
	ASSR = 0;
	TIMSK2 = 0;
	TIFR2 = 0;
	TCCR2B = _BV(CS20);			// Start timer

	reset();
}

void rgbLED_refresh()
{
	uint8_t cnt = 8;
	uint32_t *p = &rgbLED[0];
	// Time sensitive
	cli();
	do {
		uint32_t clr = *p++;
		send(GREEN_888(clr));
		send(RED_888(clr));
		send(BLUE_888(clr));
	} while (--cnt);
	sei();
}
