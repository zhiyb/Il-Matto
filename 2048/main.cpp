#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <tft.h>
#include <rtouch.h>
#include <adcrequest.h>
#include <eemem.h>
#include "common.h"
#include "2048.h"

adcRequest_t adcReq;
rTouch touch(&adcReq);

ISR(ADC_vect, ISR_NOBLOCK)
{
	adcReq.isr();
}

void init()
{
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	tft::init();
	tft::setOrient(tft::Portrait);
	tft::clean();
	stdout = tft::devout();
	tft::setBGLight(true);
	touch.init();
	sei();

	touch.calibrate();
	eeprom_counter_increment();
	srand(eeprom_counter());
	eeprom_done();
}

int main()
{
	init();
	game2048 game;

	game.enter();
	for (;;)
		game.pool();

	return 1;
}
