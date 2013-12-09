#include <avr/io.h>
#include <util/delay.h>
#include "rotary.h"

//#define ACCURACY

#define ROE_1 (ROE_A1 | ROE_B1)
#define ROE_2 (ROE_A2 | ROE_B2)
#define delay() _delay_ms(3);

uint8_t prev;

void ROE_init(void)
{
	// Input port: Pull up
	ROE_DDR &= ROE_1 | ROE_2 | ROE_S1 | ROE_S2;
	ROE_PORT |= ROE_1 | ROE_2 | ROE_S1 | ROE_S2;
reget:
	prev = ROE_PIN & ~ROE_S1 & ~ROE_S2;
	delay();
	if (prev != (ROE_PIN & ~ROE_S1 & ~ROE_S2))
		goto reget;
}

uint8_t ROE_get(void)
{
	uint8_t now = ROE_PIN, pre = prev;
	if (now == prev)
		return ROE_N;
	delay();
	now = ROE_PIN;
	if (now == prev)
		return ROE_N;
	if (!(now & ROE_S1))
		return ROE_SW1;
	if (!(now & ROE_S2))
		return ROE_SW2;
	prev = now;
	if ((now & ROE_1) == (pre & ROE_1))
		goto ROE2;
	switch (pre & ROE_1) {
	case ROE_A1:
#ifdef ACCURACY
		return (now & ROE_1) == 0 ? ROE_CW1 : ROE_CCW1;
#else
		return ROE_N;
#endif
	case ROE_B1:
#ifdef ACCURACY
		return (now & ROE_1) == 0 ? ROE_CCW1 : ROE_CW1;
#else
		return ROE_N;
#endif
	case ROE_A1 | ROE_B1:
		return (now & ROE_1) == ROE_A1 ? ROE_CW1 : ROE_CCW1;
	case 0:
		return (now & ROE_1) == ROE_A1 ? ROE_CCW1 : ROE_CW1;
	}
ROE2:
	if ((now & ROE_2) == (pre & ROE_2))
		return ROE_N;
	switch (pre & ROE_2) {
	case ROE_A2:
#ifdef ACCURACY
		return (now & ROE_2) == 0 ? ROE_CW2 : ROE_CCW2;
#else
		return ROE_N;
#endif
	case ROE_B2:
#ifdef ACCURACY
		return (now & ROE_2) == 0 ? ROE_CCW2 : ROE_CW2;
#else
		return ROE_N;
#endif
	case ROE_A2 | ROE_B2:
		return (now & ROE_2) == ROE_A2 ? ROE_CW2 : ROE_CCW2;
	case 0:
		return (now & ROE_2) == ROE_A2 ? ROE_CCW2 : ROE_CW2;
	}
	return ROE_N;
}
