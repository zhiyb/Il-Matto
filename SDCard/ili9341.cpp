#include <avr/io.h>
#include <util/delay.h>
#include "ili9341.h"

#define LOW(b)	TFT_WCTRL &= ~(b)
#define HIGH(b)	TFT_WCTRL |= (b)
#define SEND() TFT_PDATA = 0xFF
#define RECV() { \
	TFT_PDATA = 0x00; \
	TFT_WDATA = 0xFF; \
}

void ili9341::init(void)
{
	uint8_t c;
	uint16_t r;

	MCUCR |= 0x80;			// Disable JTAG
	MCUCR |= 0x80;			// Thanks to someone point that out

	TFT_PCTRL = 0xFF & ~TFT_FMK;
	TFT_WCTRL = 0xFF & ~TFT_BLC;	// Disable background light
	RECV();
	LOW(TFT_RST);			// Hardware reset
	LOW(TFT_CS);
	HIGH(TFT_WR);
	HIGH(TFT_RD);
	HIGH(TFT_RS);
	HIGH(TFT_VSY);
	_delay_us(10);			// Min: 10us
	HIGH(TFT_RST);
	_delay_ms(120);
	send(1, 0x28);		// Display OFF
	send(1, 0x11);		// Sleep Out
	_delay_ms(120);
	send(1, 0x34);		// Tearing Effect Line OFF
	send(1, 0x38);		// Idle Mode OFF
	send(1, 0x13);		// Normal Display Mode ON
	send(1, 0x20);		// Display Inversion OFF
	send(1, 0x3A);		// COLMOD: Pixel Format Set
	send(0, 0x55);		// 16 bits/pixel
	send(1, 0x36);		// Memory Access Control
	send(0, 0x48);		// Column Adress Order, BGR
	send(1, 0x2C);		// Memory Write
	for (r = 0; r < 320; r++)	// Black screen
		for (c = 0; c < 240; c++) {
			send(0, 0x00);
			send(0, 0x00);
			send(0, 0x00);
		}
	send(1, 0x29);		// Display On
}
