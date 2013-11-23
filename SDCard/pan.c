#include <avr/io.h>

void init_pan(void)
{
	TCCR2A = _BV(COM2A0) | _BV(COM2A1);	// Set OC1A on Compare Match
	TCCR2A |= _BV(COM2B0) | _BV(COM2B1);	// Set OC1B on Compare Match
	TCCR2A |= _BV(WGM21) | _BV(WGM20);	// PWM, Fast, 0xFF, BOTTOM
	TCCR2B = _BV(CS20);			// clk/1 prescaler
	ASSR = 0;				// I/O clock
	TIMSK2 = 0;				// No interrupts
	TCNT2 = 0x00;				// Counter 1
	OCR2A = 0xFF;				// Compare register
	DDRD |= _BV(6) | _BV(7);		// Output
}

void pan(uint8_t v)
{
	OCR2A = (100 - v) * 0xFF / 100;		// Duty cycle
	OCR2B = v * 0xFF / 100;			// Duty cycle
}

void novol(void)
{
	OCR2A = 0xFF;
	OCR2B = 0xFF;
}
