#include "adc.h"

void adc_init(uint8_t channel)
{
	ADMUX = channel;
	// Sample rate F_CPU / 128 ~ 84kHz (< 200kHz)
	ADCSRA = _BV(ADIF) | 7;
	adc_enable(1);
}

void adc_start(void)
{
	ADCSRA |= _BV(ADSC);
}

void adc_enable(const uint8_t e)
{
	if (e)
		ADCSRA |= _BV(ADEN);
	else
		ADCSRA &= ~_BV(ADEN);
}

uint16_t adc_read(void)
{
	while (ADCSRA & _BV(ADSC));
	return ADC;
}
