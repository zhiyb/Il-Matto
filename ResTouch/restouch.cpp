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

void ResTouch::init(void)
{
	function(Detection);
}

void ResTouch::mode(Functions func)
{
	switch (func) {
	case Detection:
		//DIDR0 &= ~(RESTOUCH_XP | RESTOUCH_YP);
		ADCSRA &= ~_BV(ADEN);
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~(RESTOUCH_XP | RESTOUCH_YP);
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		break;
	case ReadX:
		//DIDR0 &= ~RESTOUCH_XP;
		//DIDR0 |= RESTOUCH_YP;
		RESTOUCH_DDRM |= RESTOUCH_XM;
		RESTOUCH_DDRM &= ~RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP |= RESTOUCH_XP;
		RESTOUCH_DDRP &= ~RESTOUCH_YP;
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		adcInit(RESTOUCH_XC);
		break;
	case ReadY:
		//DIDR0 &= ~RESTOUCH_YP;
		//DIDR0 |= RESTOUCH_XP;
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~RESTOUCH_XP;
		RESTOUCH_DDRP |= RESTOUCH_YP;
		RESTOUCH_PORTP &= ~RESTOUCH_XP;
		RESTOUCH_PORTP |= RESTOUCH_YP;
		adcInit(RESTOUCH_YC);
		break;
	};
}

uint16_t ResTouch::function(Functions func)
{
	switch (func) {
	case Detection:
		return !(RESTOUCH_PINP & RESTOUCH_XP);
	case ReadX:
		return adcRead();
	case ReadY:
		return adcRead();
	};
	return 0;
}
