#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

#define RATE 5
#define QUICK 500
#define MINIMUM 800
#define MAXIMUM 6250

uint8_t volatile overflow = 0, quick = 0;
uint16_t volatile counter, maximum;

void inline Timer_quick(void)
{
	quick = 1;
}

void inline Timer_normal(void)
{
	quick = 0;
}

ISR(TIMER0_COMPA_vect)
{
	if (quick) {
		if (counter++ < QUICK)
			return;
	} else
		if (counter++ < maximum)
			return;
	counter = 0;
	/*if (PORTB & 0x80)
		PORTB &= ~0x80;
	else
		PORTB |= 0x80;*/
	overflow = 1;
}

EMPTY_INTERRUPT(BADISR_vect);

void Timer_init(void)
{
	// Using Timer0
	Timer_disable();
	sei();
	TCCR0A = 1 << WGM01;	// Clear Timer on Compare Match (CTC) mode
	OCR0A = 0xF0;		// Compare Register A (0.16ms)
	TCNT0 = 0x00;		// Timer0 Register
	TIMSK0 = 1 << OCIE0A;	// Enable Timer0 Output Compare Match A
	Timer_reset();
}

void Timer_reset(void)
{
	Timer_disable();
	TCNT0 = 0x00;		// Timer0 Register
	counter = 0;
	maximum = MAXIMUM;
	overflow = 0;
}

void Timer_faster(void)
{
	if (maximum > MINIMUM)
		maximum -= RATE;
	else
		maximum = MINIMUM;
}

uint16_t inline Timer_speed(void)
{
	return MAXIMUM - maximum;
}
