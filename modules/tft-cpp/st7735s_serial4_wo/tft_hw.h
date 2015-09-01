/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_ILI9341_H
#define TFT_ILI9341_H

#include <avr/io.h>
#include <util/delay.h>
#include <macros.h>
#include "connection.h"

// With respect to landscape orientation
#define TFT_SIZE_HEIGHT	128
#define TFT_SIZE_WIDTH	128
#define TFT_DEF_ORIENT	tft::Portrait

#define TFT_DIR		CONCAT_E(DDR, TFT_PORT)
#define TFT_WRITE	CONCAT_E(PORT, TFT_PORT)
#define TFT_READ	CONCAT_E(PIN, TFT_PORT)

namespace tft
{
	enum Orientation {Landscape = 0, Portrait, \
		FlipLandscape, FlipPortrait, \
		BMPLandscape, BMPPortrait, \
		BMPFlipLandscape, BMPFlipPortrait, \
		Orientations};

	extern uint8_t orient;
}

namespace tfthw
{
	static inline void cmd(uint8_t d);
	static inline void data(uint8_t d);
	
	static inline void idle(bool e) {cmd(0x38 + e);}
	static inline void sleep(bool e) {cmd(0x10 + e);}
	static inline void inversion(bool e) {cmd(0x20 + e);}
}

// Port
namespace tfthw
{
	static inline void init();
	static inline void setBGLight(bool ctrl);
	static inline void setOrient(uint8_t o);
	static inline void setPage(const uint16_t start, const uint16_t end);
	static inline void setColumn(const uint16_t start, const uint16_t end);
	// 0x2c Write, 0x2e Read, 0x3c / 0x3e Continue, 0x00 NOP
	static inline void memWrite() {cmd(0x2c);}
	static inline void write(const uint8_t d) {data(d);}
	static inline void write16(const uint16_t c) {write(c >> 8); write(c & 0xff);}

#ifdef TFT_VERTICAL_SCROLLING
	static inline void setVSStart(const uint16_t addr);
	static inline void setVSDefinition(const uint16_t tfa, \
			const uint16_t vsa, const uint16_t bfa);
#endif
}

// Defined as inline to excute faster

#ifdef TFT_VERTICAL_SCROLLING
static inline void tfthw::setVSStart(uint16_t addr)
{
	static const uint8_t offset[tft::Orientations] = {1, 1, 3, 3};
	cmd(0x37);	// Vertical Scrolling Start Address
	write16(addr + offset[tft::orient]);
}

static inline void tfthw::setVSDefinition(const uint16_t tfa, \
		const uint16_t vsa, const uint16_t bfa)
{
	static const uint8_t offset[tft::Orientations] = {1, 1, 3, 3};
	cmd(0x33);	// Vertical Scrolling Definition
	write16(tfa + offset[tft::orient]);	// Top Fixed Area
	write16(vsa);	// Vertical Scrolling Area
	write16(bfa + 3);	// Bottom Fixed Area
}
#endif

static inline void tfthw::setColumn(const uint16_t start, const uint16_t end)
{
	static const uint8_t offset[tft::Orientations] = {1, 2, 3, 2};
	cmd(0x2a);	// Set column address
	write16(start + offset[tft::orient]);
	write16(end + offset[tft::orient]);
}

static inline void tfthw::setPage(const uint16_t start, const uint16_t end)
{
	static const uint8_t offset[tft::Orientations] = {2, 1, 2, 3};
	cmd(0x2b);	// Set page(row) address
	write16(start + offset[tft::orient]);
	write16(end + offset[tft::orient]);
}

static inline void tfthw::setOrient(uint8_t o)
{
	using namespace tft;
	static const uint8_t base = 0x08;
	static const uint8_t MY = 1U << 7, MX = 1U << 6, MV = 1U << 5;
	static uint8_t orient[Orientations];
	orient[Landscape]		= base | MX | MV;
	orient[Portrait]		= base;
	orient[FlipLandscape]		= orient[Landscape] ^ (MY | MX);
	orient[FlipPortrait]		= orient[Portrait] ^ (MY | MX);
	orient[BMPLandscape]		= orient[Landscape] ^ MX;
	orient[BMPFlipLandscape]	= orient[FlipLandscape] ^ MX;
	orient[BMPPortrait]		= orient[Portrait] ^ MY;
	orient[BMPFlipPortrait]		= orient[FlipPortrait] ^ MY;
	cmd(0x36);			// Memory Access Control
	data(orient[o]);
}

static inline void tfthw::cmd(uint8_t d)
{
	TFT_WRITE &= ~TFT_DCX;
	data(d);
	TFT_WRITE |= TFT_DCX;
}

static inline void tfthw::data(uint8_t d)
{
	TFT_WRITE &= ~TFT_CS;
	UDR1 = d;
	while (!(UCSR1A & _BV(TXC1)));
	UCSR1A = _BV(TXC1);
	TFT_WRITE |= TFT_CS;
}

static inline void tfthw::setBGLight(bool ctrl)
{
	if (ctrl)
		TFT_WRITE |= TFT_LED;
	else
		TFT_WRITE &= ~TFT_LED;
}

static inline void tfthw::init()
{
	uint8_t c;
	uint16_t r;

	if (TFT_WRITE == PORTC) {
		MCUCR |= 0x80;			// Disable JTAG
		MCUCR |= 0x80;
	}

	TFT_DIR |= TFT_LED | TFT_MOSI | TFT_SCK | TFT_CS | TFT_DCX | TFT_RST;
	TFT_WRITE |= TFT_MOSI | TFT_SCK | TFT_CS | TFT_DCX | TFT_RST;
	TFT_WRITE &= ~(TFT_LED);	// Disable background light

	// Initialise SPI interface
	UCSR1B = /*_BV(RXEN1) |*/ _BV(TXEN1);
	UCSR1C = _BV(UMSEL11) | _BV(UMSEL10);
	UBRR1 = 0;
	UCSR1A = 0xff;	// Clear flags

	TFT_WRITE &= ~TFT_RST;	// Hardware reset
	TFT_WRITE |= TFT_CS;
	_delay_us(10);		// Min: 10us
	TFT_WRITE |= TFT_RST;
	_delay_ms(120);
	cmd(0x28);		// Display OFF
	cmd(0x11);		// Sleep Out
	_delay_ms(120);
	cmd(0x34);		// Tearing Effect Line OFF
	cmd(0x38);		// Idle Mode OFF
	cmd(0x13);		// Normal Display Mode ON
	cmd(0x20);		// Display Inversion OFF
	cmd(0x3A);		// COLMOD: Pixel Format Set
	data(0x55);		// 16 bits/pixel
	cmd(0x36);		// Memory Access Control
	data(0x48);		// Column Adress Order, BGR
	cmd(0x2C);		// Memory Write
	for (r = 0; r < 320; r++)	// Black screen
		for (c = 0; c < 240; c++) {
			data(0x00);
			data(0x00);
		}
	cmd(0x29);		// Display On
}

#endif
