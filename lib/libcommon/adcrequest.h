/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef ADCREQUEST_H
#define ADCREQUEST_H

#include <avr/io.h>
#include "request.h"

class adcRequest_t : public request_t
{
public:
	adcRequest_t()
	{
		// Clear interrupt, Interrupt enable
		// Clock rate F_CPU / 128 ~ 94kHz (< 200kHz)
		ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADIE) | 7;
	}

protected:
	virtual void execute(req_t *req)
	{
		ADMUX = req->param;
		ADCSRA |= _BV(ADSC);
	}
	virtual bool available() {return !(ADCSRA & _BV(ADSC));}
	virtual uint16_t getResult() {return ADC;}
	virtual uint8_t getParam() {return ADMUX & 0x1F;}
};

#endif
