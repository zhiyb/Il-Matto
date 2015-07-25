// Author: Yubo Zhi (normanzyb@gmail.com)

#include <avr/io.h>
#include <macros.h>
#include "rgbled.h"

void init()
{
	rgbLED_init();
	rgbLED_refresh();

	// Animation timer 1, 256Hz
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	TCNT1 = 0;
	OCR1A = F_CPU / 256UL;
	TIMSK1 = 0;
	TIFR1 = 0;
	TCCR1B = _BV(WGM12) | _BV(CS10);
}

uint8_t animation()
{
	uint8_t ret = TIFR1 & _BV(OCF1A);
	if (ret)
		TIFR1 |= _BV(OCF1A);
	return ret;
}

int main()
{
	init();

	uint8_t i = 0, j = 0;
	int8_t d = 1;
	uint8_t cnt;
loop:
	cnt = RGBLED_NUM;
	do {
		cnt--;
		switch (j) {
		case 0:
			rgbLED[cnt] = COLOUR888(i, 0, 0);
			break;
		case 1:
			rgbLED[cnt] = COLOUR888(i, i, 0);
			break;
		case 2:
			rgbLED[cnt] = COLOUR888(0, i, 0);
			break;
		case 3:
			rgbLED[cnt] = COLOUR888(0, i, i);
			break;
		case 4:
			rgbLED[cnt] = COLOUR888(0, 0, i);
			break;
		case 5:
			rgbLED[cnt] = COLOUR888(i, 0, i);
			break;
		}
	} while (cnt);
	if (i == 255 && d == 1)
		d = -1;
	else if (i == 0 && d == -1) {
		d = 1;
		j = j == 5 ? 0 : j + 1;
	}
	i += d;
	rgbLED_refresh();
	while (!animation());
	goto loop;
	return 1;
}
