#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <eemem.h>
#include "tft.h"
#include "sound.h"
#include "display.h"
#include "tetris.h"
#include "timer.h"
#include "uart.h"

void init(void)
{
	DDRB &= 0xF3;			// Disable USB Port
	PORTB &= 0xF3;
	DDRB |= 0x80;			// LED
	PORTB |= 0x80;
	Sound_init();
	//ROE_init();
	Uart_init();
	puts("Tetris: UART control interface");
	TFT_init();
	Timer_init();
	Tetris_mem_init();
	eeprom_counter_increment();
	srand(eeprom_counter());
	eeprom_done();
	Print_frame();
	Tetris_init();
/*	#define BAUD 115200
	#include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0A = USE_2X << U2X0;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
*/}

int main(void)
{
	init();

start:
	Print_object();
get:
	switch (Uart_readch()) {
	case 'w':
		Clean_object();
		Tetris_rotate(0);
		break;
	case 'W':
		Clean_object();
		Tetris_rotate(1);
		break;
	case 'd':
		Clean_object();
		Tetris_move(1);
		break;
	case 'a':
		Clean_object();
		Tetris_move(-1);
		break;
	case ' ':
	case 'q':
		Tetris_pause();
		goto get;;
	case 's':
		Timer_quick();
		goto get;;
	default:
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
	}
	goto start;

	return 1;
}
