#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <tft.h>
#include <portraitlist.h>
#include <landscapelist.h>
#include <rtouch.h>
#include <eemem.h>
#include <adc.h>
#include <colours.h>
#include "menu.h"

//#define LANDSCAPE

tft_t tft;
rTouch touch(&tft);
#ifdef LANDSCAPE
LandscapeList l(&tft);
#else
PortraitList l(&tft);
#endif

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	adc_init();
	adc_enable();
	tft.init();
#ifdef LANDSCAPE
	tft.setOrient(tft.FlipLandscape);
#else
	tft.setOrient(tft.Portrait);
#endif
	tft.setBackground(0x0000);
	tft.setForeground(0x667F);
	tft.clean();
	stdout = tftout(&tft);
	touch.init();
	sei();

	tft.setBGLight(true);
	touch.calibrate();
	eeprom_first_done();
}

int main(void)
{
	init();

	tft.clean();
	tft.setForeground(0x0000);

	l.refresh();
	l.display(&menuRoot);
	l.setScroll(260);
	//l.refresh();

	rTouch::coord_t prev = {-1, -1};
pool:
	if (touch.pressed()) {
		rTouch::coord_t res = touch.position();
		if (!touch.pressed()) {
			prev.x = -1;
			prev.y = -1;
			goto pool;
		}
#ifdef LANDSCAPE
		if (prev.y > 0 && (int16_t)l.scroll() - res.x + prev.x > 0)
			l.setScroll((int16_t)l.scroll() - res.x + prev.x);
#else
		if (prev.x > 0 && (int16_t)l.scroll() - res.y + prev.y > 0)
			l.setScroll((int16_t)l.scroll() - res.y + prev.y);
#endif
		prev = res;
	} else {
		using namespace colours::b16;
#ifdef DEBUG
#ifdef LANDSCAPE
		tft.rectangle(tft.topEdge() - 1, 0, 1, tft.height(), White);
		tft.rectangle(tft.bottomEdge(), 0, 1, tft.height(), White);
#else
		tft.rectangle(0, tft.topEdge() - 1, tft.width(), 1, White);
		tft.rectangle(0, tft.bottomEdge(), tft.width(), 1, White);
		tft.rectangle(0, tft.bottomEdge() - 1, tft.width(), 1, Black);
#endif
#endif
		prev.x = -1;
		prev.y = -1;
	}
	goto pool;

	return 1;
}
