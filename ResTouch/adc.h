#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

namespace adc
{
	void init(uint8_t channel);
	void enable(bool e);
	void start(void);
	uint16_t read(void);
}

#endif
