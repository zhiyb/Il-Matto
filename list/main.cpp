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
#include <adcrequest.h>
#include <colours.h>
#include "menu.h"

//#define LANDSCAPE

adcRequest_t adcReq;
rTouch touch(&adcReq);
#ifdef LANDSCAPE
LandscapeList l;
#else
PortraitList l;
#endif

ISR(ADC_vect, ISR_NOBLOCK)
{
	adcReq.isr();
}

void init(void)
{
	DDRB |= _BV(7);			// LED
	PORTB |= _BV(7);

	tft::init();
#ifdef LANDSCAPE
	tft::setOrient(tft::FlipLandscape);
#else
	tft::setOrient(tft::Portrait);
#endif
	tft::background = 0x0000;
	tft::foreground = 0x667F;
	tft::clean();

	stdout = tft::devout();
	touch.init();
	sei();

	tft::setBGLight(true);
	touch.calibrate();
	eeprom_done();
}

int main(void)
{
	init();

	tft::clean();
	tft::foreground = 0x0000;

	l.refresh();
	l.setRootItem(&menu::root::item);
	l.display(&menu::root::item);

pool:
	l.pool(&touch);
	goto pool;

	return 1;
}
