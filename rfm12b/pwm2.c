#include <avr/io.h>
#include "pwm2.h"

void pwm2_init()
{
	pwm2_enable(0);
	// Timer 2, fast PWM, 8-bit
	TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
	TCCR2B = 0;

	// OC2A port
	DDRD |= _BV(7);
}

void pwm2_set(const uint8_t value)
{
	OCR2A = value;
}

uint8_t pwm2_get()
{
	return OCR2A;
}

void pwm2_enable(uint8_t e)
{
	// Prescaler of 1
	if (e)
		TCCR2B = _BV(CS20);
	else
		TCCR2B = 0;
}
