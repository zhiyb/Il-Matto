/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef ADC_H
#define ADC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define adc_enable()	ADCSRA |= _BV(ADEN)
#define adc_disable()	ADCSRA &= ~_BV(ADEN)

void adc_init(void);
void adc_register_ISR(void (*handler)(uint8_t channel, uint16_t result));
void adc_request(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif
