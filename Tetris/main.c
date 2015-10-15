#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <eemem.h>
#include "rotary.h"
#include "tft.h"
#include "sound.h"
#include "display.h"
#include "tetris.h"
#include "timer.h"

void init(void)
{
	DDRB &= 0xF3;			// Disable USB Port
	PORTB &= 0xF3;
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	Sound_init();
	ROE_init();
	TFT_init();
	Timer_init();
	Tetris_mem_init();
	eeprom_counter_increment();
	srand(eeprom_counter());
	eeprom_done();
	Print_frame();
	Tetris_init();
}

int main(void)
{
	init();

start:
	Print_object();
get:
	switch (ROE_get()) {
	case ROE_N:
		if (!overflow)
			goto get;
		overflow = 0;
		Clean_object();
		if (!Tetris_drop()) {
			Timer_normal();
			Print_object();
			Tetris_place();
			if (!Tetris_generate())
				Tetris_gameover();
			Print_next();
		}
		break;
	case ROE_CW1:
		Clean_object();
		Tetris_rotate(0);
		break;
	case ROE_CCW1:
		Clean_object();
		Tetris_rotate(1);
		break;
	case ROE_CW2:
		Clean_object();
		Tetris_move(1);
		break;
	case ROE_CCW2:
		Clean_object();
		Tetris_move(-1);
		break;
	case ROE_SW1:
		Tetris_pause();
		goto get;;
	case ROE_SW2:
		Timer_quick();
		goto get;;
	}
	goto start;

	return 1;
}
