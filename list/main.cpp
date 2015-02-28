#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <portraitlist.h>
#include <landscapelist.h>
#include <restouch.h>
#include <eemem.h>
#include "menu.h"

//#define LANDSCAPE

tft_t tft;
ResTouch touch(&tft);
#ifdef LANDSCAPE
LandscapeList l(&tft);
#else
PortraitList l(&tft);
#endif

void init(void)
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
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

	ResTouch::coord_t prev = {-1, -1};
pool:
	if (touch.detect()) {
		ResTouch::coord_t res = touch.read();
		if (!touch.detect()) {
			prev.x = -1;
			goto pool;
		}
		if (prev.x > 0 && (int16_t)l.scroll() - res.y + prev.y > 0)
			l.setScroll((int16_t)l.scroll() - res.y + prev.y);
		prev = res;
	} else
		prev.x = -1;
	goto pool;

#if 0
#ifdef LANDSCAPE
	tft.rectangle(tft.topEdge() - 1, 0, 1, tft.height(), 0xF800);
	tft.rectangle(tft.bottomEdge(), 0, 1, tft.height(), 0xF800);
#else
	tft.rectangle(0, tft.topEdge() - 1, tft.width(), 1, 0xF800);
	tft.rectangle(0, tft.bottomEdge(), tft.width(), 1, 0xF800);
#endif
#endif

	uint16_t max = l.maxScroll(), v = 0;
inc:
	l.setScroll(v);
	_delay_ms(10);
	if (++v < max)
		goto inc;
	_delay_ms(1000);
dec:
	l.setScroll(v);
	_delay_ms(10);
	if (--v > 0)
		goto dec;
	_delay_ms(1000);
	goto inc;

	return 1;
}
