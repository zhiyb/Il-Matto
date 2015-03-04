/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

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
	DDRB |= _BV(7);			// LED
	PORTB |= _BV(7);
	DDRD |= _BV(6);
	PORTD |= _BV(6);
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
	l.setRootItem(&item_root);
	l.display(&item_root);

pool:
	l.pool(&touch);
	goto pool;

	return 1;
}
