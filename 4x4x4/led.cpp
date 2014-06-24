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

	TCCR1A = 0;
	TCCR1B = 0;					// Stop
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = 1470;					// 85Hz
	//OCR1A = 368;					// 85Hz * 4
	TIMSK1 = _BV(OCIE1A);
	TIFR1 = 0xFF;
	//TCCR1B = _BV(WGM12) | _BV(CS11) | _BV(CS10);
	TCCR1B = _BV(WGM12) |  _BV(CS10);				// / 1
}

void led::fill(bool state)
{
	for (uint8_t i = 0; i < 4; i++)
		buff[i] = state ? 0xFFFF : 0x0000;
}

ISR(TIMER1_COMPA_vect)
{
	using namespace led;

	PORTC = 0x00;
	PORTB = (buff[level] >> 8) & 0xFF;
	PORTD = buff[level] & 0xFF;
	PORTC = 1 << level;

	level = level == 3 ? 0 : level + 1;
}
