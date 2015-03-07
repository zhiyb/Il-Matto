/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <rgbconv.h>
#include <rtouch.h>
#include <eemem.h>
#include <adc.h>

//#define AUTO_COLOUR

tft_t tft;
rTouch touch(&tft);

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	adc_init();
	adc_enable();
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setBackground(0x0000);
	tft.setForeground(0x667F);
	stdout = tftout(&tft);
	touch.init();
	sei();

	tft.setBGLight(true);
	touch.calibrate();
	tft.clean();
	eeprom_first_done();
}

int main(void)
{
	init();

start:
	tft.setOrient(tft.Portrait);
	tft.clean();
	tft.setZoom(1);
	puts_P(PSTR("*** Touch ***"));

#ifdef AUTO_COLOUR
	bool pressed = false;
	uint8_t cnt = 0;
#endif
	uint16_t clr = 0xFFFF;
	static uint16_t c[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0x07FF, 0xF81F, 0xFFFF, 0x0000};
loop:
	if (touch.pressed()) {
		rTouch::coord_t res = touch.position();
		if (res.x <= -20) {
#if 1
			int16_t spl = tft.height() / 7;
			clr = c[res.y / spl];
#else
			uint8_t mv = res.y * 16 / tft.height();
			clr = (0x001F << mv) | (((uint32_t)0x001F << mv) >> 16);
#endif
		} else if (res.x > (int16_t)(tft.width() + 20))
			tft.clean();
		else {
#ifdef AUTO_COLOUR
			pressed = true;
#endif
			tft.point(res.x, res.y, clr);
		}
#ifdef AUTO_COLOUR
	} else if (pressed) {
		clr = c[cnt++];
		if (cnt == 7)
			cnt = 0;
		pressed = false;
#endif
	}
	goto loop;
	goto start;

	return 1;
}
