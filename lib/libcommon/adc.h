#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

void adc_init(uint8_t channel);
void adc_enable(const uint8_t e);
void adc_start(void);
uint16_t adc_read(void);

#endif
