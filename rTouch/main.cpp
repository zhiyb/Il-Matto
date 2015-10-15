/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <rtouch.h>
#include <eemem.h>
#include <capture.h>
#include <adcrequest.h>

#define AUTO_COLOUR

adcRequest_t adcReq;
rTouch touch(&adcReq);

ISR(ADC_vect, ISR_NOBLOCK)
{
	adcReq.isr();
}

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft::init();
	tft::setOrient(tft::Portrait);
	tft::background = 0x0000;
	tft::foreground = 0x667F;
	stdout = tft::devout();
	touch.init();
	sei();

	tft::setBGLight(true);
	capture::init();
	capture::enable();
	touch.calibrate();
	tft::clean();
	eeprom_done();
}

int main(void)
{
	init();

start:
	tft::setOrient(tft::Portrait);
	tft::clean();
	tft::zoom = 1;
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
			int16_t spl = tft::height / 7;
			clr = c[res.y / spl];
		} else if (res.x > (int16_t)(tft::width + 20))
			tft::clean();
		else {
#ifdef AUTO_COLOUR
			pressed = true;
#endif
			tft::point(res.x, res.y, clr);
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
