#include <avr/io.h>
#include <avr/interrupt.h>
#include "led.h"

volatile uint16_t led::buff[4];
uint8_t level;

void led::init(void)
{
	level = 0;
	fill(false);

	DDRB = 0xFF;
	DDRD = 0xFF;
	PORTB = 0xFF;
	PORTD = 0xFF;
	DDRC = 0x0F;
	PORTC = 0;

	enable(false);
	enablePWM(true);
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = Top;
	OCR1B = 0;
	TIFR1 = 0xFF;
}

void led::fill(bool state)
{
	for (uint8_t i = 0; i < 4; i++)
		buff[i] = state ? 0xFFFF : 0x0000;
}

void led::enable(bool e)
{
	if (e)
		TCCR1B = _BV(WGM12) |  _BV(CS10);
	else
		TCCR1B = 0;
}

bool led::enabled(void)
{
	return TCCR1B != 0;
}

void led::enablePWM(bool e)
{
	if (e)
		TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
	else
		TIMSK1 = _BV(OCIE1A);
}

bool led::PWMEnabled(void)
{
	return TIMSK1 & _BV(OCIE1B);
}

void led::setPWM(uint16_t pwm)
{
	OCR1B = pwm;
}

ISR(TIMER1_COMPA_vect)
{
	using namespace led;

	if (DDRC == 0x0F && PWMEnabled())
		return;
	DDRC = 0x0F;
	PORTC = 0x00;
	PORTB = (buff[level] >> 8) & 0xFF;
	PORTD = buff[level] & 0xFF;
	if (DDRC == 0x0F)
		PORTC = 1 << level;

	level = level == 3 ? 0 : level + 1;
}

ISR(TIMER1_COMPB_vect)
{
	PORTC = 0x00;
	DDRC = 0x00;
}
