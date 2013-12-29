#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rotary.h"
#include "timer.h"

//#define ACCURACY

#define ROE_1	(ROE_A1 | ROE_B1)
#define ROE_2	(ROE_A2 | ROE_B2)
#define ROE	(ROE_1 | ROE_2 | ROE_S1 | ROE_S2)
#define delay()	_delay_ms(5);

volatile uint8_t prev, res = ROE_N;

void ROE_init(void)
{
	// Input port: Pull up
	ROE_DDR &= ROE;
	ROE_PORT |= ROE;
reget:
	prev = ROE_PIN & ROE;
	delay();
	if (prev != (ROE_PIN & ROE))
		goto reget;
	
	// Pin Change Interrupt
	PCICR |= _BV(PCIE3);		// Port D: PCINT24 - PCINT31
	PCIFR |= _BV(PCIF3);		// Clear flag
	PCMSK3 = ROE;

	// Debouncing timer
	timer2_init();
}

uint8_t ROE_get(void)
{
	uint8_t tmp = res;
	res = ROE_N;
	return tmp;
}

ISR(PCINT3_vect)
{
	timer2_enable();
}

ISR(TIMER2_COMPA_vect)
{
	uint8_t now, pre = prev;
	timer2_disable();
	now = ROE_PIN & ROE;
	if (now == pre)
		return;
	if (!(now & ROE_S1)) {
		res = ROE_SW1;
		return;
	}
	if (!(now & ROE_S2)) {
		res = ROE_SW2;
		return;
	}
	prev = now;
	if ((now & ROE_1) == (pre & ROE_1))
		goto ROE2;
	switch (pre & ROE_1) {
	case ROE_A1:
#ifdef ACCURACY
		res = (now & ROE_1) == 0 ? ROE_CW1 : ROE_CCW1;
#endif
		return;
	case ROE_B1:
#ifdef ACCURACY
		res = (now & ROE_1) == 0 ? ROE_CCW1 : ROE_CW1;
#endif
		return;
	case ROE_A1 | ROE_B1:
		res = (now & ROE_1) == ROE_A1 ? ROE_CW1 : ROE_CCW1;
		return;
	case 0:
		res = (now & ROE_1) == ROE_A1 ? ROE_CCW1 : ROE_CW1;
		return;
	}
ROE2:
	switch (pre & ROE_2) {
	case ROE_A2:
#ifdef ACCURACY
		res = (now & ROE_2) == 0 ? ROE_CW2 : ROE_CCW2;
#endif
		return;
	case ROE_B2:
#ifdef ACCURACY
		res = (now & ROE_2) == 0 ? ROE_CCW2 : ROE_CW2;
#endif
		return;
	case ROE_A2 | ROE_B2:
		res = (now & ROE_2) == ROE_A2 ? ROE_CW2 : ROE_CCW2;
		return;
	case 0:
		res = (now & ROE_2) == ROE_A2 ? ROE_CCW2 : ROE_CW2;
	}
}
