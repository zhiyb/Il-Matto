#include <avr/io.h>
#include "restouch.h"

// Interface functions
// Ports	PORTA		PORTB
// Function	X+	Y+	X-	Y-
// Detection	DINPUT	High-Z	High-Z	VSS
// Read X	VCC	ADC	VSS	High-Z
// Read Y	ADC	VCC	High-Z	VSS

static void adcInit(uint8_t channel)
{
	ADMUX = channel;
	// Sample rate F_CPU / 64 (< 200kHz)
	ADCSRA = _BV(ADEN) | _BV(ADIF) | _BV(ADPS2) | _BV(ADPS1);
	ADCSRA |= _BV(ADSC);
}

static uint16_t adcRead(void)
{
	while (ADCSRA & _BV(ADSC));
	return ADC;
}

ResTouch::ResTouch(tft_t *tft)
{
	this->tft = tft;
}

void ResTouch::init(void)
{
	mode(Detection);
}

// For FAST operation, Detection -> ReadY -> ReadX -> Detection ONLY!
void ResTouch::mode(Functions func)
{
	switch (func) {
	case Detection:
		ADCSRA &= ~_BV(ADEN);
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~(RESTOUCH_XP | RESTOUCH_YP);
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_XP;
		DIDR0 |= RESTOUCH_YP;
		break;
	case ReadY:
#ifndef FAST
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~RESTOUCH_XP;
#endif
		RESTOUCH_DDRP |= RESTOUCH_YP;
		RESTOUCH_PORTP &= ~RESTOUCH_XP;
		RESTOUCH_PORTP |= RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_YP;
		DIDR0 |= RESTOUCH_XP;
		adcInit(RESTOUCH_YC);
		break;
	case ReadX:
		RESTOUCH_DDRM |= RESTOUCH_XM;
		RESTOUCH_DDRM &= ~RESTOUCH_YM;
#ifndef FAST
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
#endif
		RESTOUCH_DDRP |= RESTOUCH_XP;
		RESTOUCH_DDRP &= ~RESTOUCH_YP;
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_XP;
		DIDR0 |= RESTOUCH_YP;
		adcInit(RESTOUCH_XC);
		break;
	};
}

uint16_t ResTouch::function(Functions func)
{
	switch (func) {
	case Detection:
		return !(RESTOUCH_PINP & RESTOUCH_XP);
	case ReadX:
	case ReadY:
		return adcRead();
	};
	return 0;
}

ResTouch::result_t ResTouch::read(void)
{
	result_t res;
	mode(ReadY);
	res.y = function(ReadY);
	mode(ReadX);
	res.x = function(ReadX);
	mode(Detection);
	return res;
}

void ResTouch::calibration(void)
{
}
