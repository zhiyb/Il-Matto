/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/interrupt.h>
#include <stdlib.h>
#include "adc.h"

// Vref: AREF, Right adjust
#define ADC_SET_CHANNEL(channel)	ADMUX = channel
#define ADC_BUSY()			(ADCSRA & _BV(ADSC))

static struct handler_t
{
	struct handler_t *next;
	void (*handler)(uint8_t channel, uint16_t result);
} *handlers = 0;

static struct request_t
{
	struct request_t *next;
	uint8_t channel;
} *requests = 0;

static struct request_t *request_dequeue(void)
{
	struct request_t *s = requests;
	if (s)
		requests = s->next;
	return s;
}

void adc_init(void)
{
	// Clear interrupt, Interrupt enable
	// Clock rate F_CPU / 128 ~ 94kHz (< 200kHz)
	ADCSRA = _BV(ADIF) | _BV(ADIE) | 7;
	handlers = 0;
	requests = 0;
}

void adc_register_ISR(void (*handler)(uint8_t channel, uint16_t result))
{
	struct handler_t *s = malloc(sizeof(struct handler_t));
	s->next = handlers;
	s->handler = handler;
	handlers = s;
}

static void adc_start(void)
{
	struct request_t *req = request_dequeue();
	if (req) {
		ADC_SET_CHANNEL(req->channel);
		free(req);
		ADCSRA |= _BV(ADSC);
	}
}

void adc_request(uint8_t channel)
{
	if (!requests && !ADC_BUSY()) {
		ADC_SET_CHANNEL(channel);
		ADCSRA |= _BV(ADSC);
		return;
	}
	struct request_t **p = &requests;
	struct request_t *s = malloc(sizeof(struct request_t));
	s->next = 0;
	s->channel = channel;
	while (*p)
		p = &(*p)->next;
	*p = s;
	if (!ADC_BUSY())
		adc_start();
}

ISR(ADC_vect, ISR_NOBLOCK)
{
	adc_start();
	uint16_t result = ADC;
	uint8_t ch = ADMUX & 0x1F;
	struct handler_t *p = handlers;
	while (p) {
		p->handler(ch, result);
		p = p->next;
	}
}
