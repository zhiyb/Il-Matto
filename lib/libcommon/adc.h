#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

void adc_init(uint8_t channel);
void adc_enable(const uint8_t e);
void adc_start(void);
uint16_t adc_read(void);

#ifdef __cplusplus
}
#endif

#endif
