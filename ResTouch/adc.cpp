#include "adc.h"

void adc::init(uint8_t channel)
{
	ADMUX = channel;
	// Sample rate F_CPU / 128 ~ 94kHz (< 200kHz)
	ADCSRA = _BV(ADIF) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
	enable(true);
}

void adc::start(void)
{
	ADCSRA |= _BV(ADSC);
}

void adc::enable(bool e)
{
	if (e)
		ADCSRA |= _BV(ADEN);
	else
		ADCSRA &= ~_BV(ADEN);
}

uint16_t adc::read(void)
{
	while (ADCSRA & _BV(ADSC));
	return ADC;
}
