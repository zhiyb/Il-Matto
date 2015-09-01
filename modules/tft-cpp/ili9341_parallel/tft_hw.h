/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_ILI9341_H
#define TFT_ILI9341_H

#include <avr/io.h>
#include <util/delay.h>
#include <macros.h>
#include <tft_defs.h>
#include "connection.h"

// With respect to landscape orientation
#define TFT_SIZE_HEIGHT	240
#define TFT_SIZE_WIDTH	320
#define TFT_DEF_ORIENT	tft::Portrait

#define TFT_PCTRL	CONCAT_E(DDR, TFT_PORT_CTRL)
#define TFT_WCTRL	CONCAT_E(PORT, TFT_PORT_CTRL)
#define TFT_RCTRL	CONCAT_E(PIN, TFT_PORT_CTRL)
#define TFT_PDATA	CONCAT_E(DDR, TFT_PORT_DATA)
#define TFT_WDATA	CONCAT_E(PORT, TFT_PORT_DATA)
#define TFT_RDATA	CONCAT_E(PIN, TFT_PORT_DATA)

namespace tfthw
{
	static inline void cmd(uint8_t dat);
	static inline void data(uint8_t dat);
	
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
	static inline void memRead() {cmd(0x2e);}
	static inline void mode(bool read);
	static inline void write(const uint8_t d) {data(d);}
	static inline void write16(const uint16_t c) {write(c >> 8); write(c & 0xff);}
	static inline uint8_t read();

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
	cmd(0x37);	// Vertical Scrolling Start Address
	write16(addr);
}

static inline void tfthw::setVSDefinition(const uint16_t tfa, \
		const uint16_t vsa, const uint16_t bfa)
{
	cmd(0x33);	// Vertical Scrolling Definition
	write16(tfa);	// Top Fixed Area
	write16(vsa);	// Vertical Scrolling Area
	write16(bfa);	// Bottom Fixed Area
}
#endif

static inline void tfthw::setColumn(const uint16_t start, const uint16_t end)
{
	cmd(0x2a);	// Set column address
	write16(start);
	write16(end);
}

static inline void tfthw::setPage(const uint16_t start, const uint16_t end)
{
	cmd(0x2b);	// Set page(row) address
	write16(start);
	write16(end);
}

static inline void tfthw::setOrient(uint8_t o)
{
	using namespace tft;
	static const uint8_t base = 0x08;
	static const uint8_t MY = 1U << 7, MX = 1U << 6, MV = 1U << 5;
	static uint8_t orient;
	orient = o & Portrait ?  base | MX : base | MV;
	if (o & Flipped)
		orient ^= MY | MX;
	if (o & BMPMode)
		orient ^= o & Portrait ? MY : MX;
	cmd(0x36);			// Memory Access Control
	data(orient);
}

static inline void tfthw::cmd(uint8_t dat)
{
	TFT_WCTRL &= ~TFT_RS;
	TFT_WDATA = dat;
	TFT_WCTRL &= ~TFT_WR;
	TFT_WCTRL |= TFT_WR;
	TFT_WCTRL |= TFT_RS;
}

static inline void tfthw::data(uint8_t dat)
{
	TFT_WDATA = dat;
	TFT_WCTRL &= ~TFT_WR;
	TFT_WCTRL |= TFT_WR;
}

static inline void tfthw::mode(bool read)
{
	if (read) {
		TFT_PDATA = 0x00;
		TFT_WDATA = 0xFF;
	} else
		TFT_PDATA = 0xFF;
}

static inline uint8_t tfthw::read()
{
	unsigned char dat;
	TFT_WCTRL &= ~TFT_RD;
	_NOP();
	dat = TFT_RDATA;
	TFT_WCTRL |= TFT_RD;
	return dat;
}

static inline void tfthw::setBGLight(bool ctrl)
{
	if (ctrl)
		TFT_WCTRL |= TFT_BLC;
	else
		TFT_WCTRL &= ~TFT_BLC;
}

static inline void tfthw::init()
{
	uint8_t c;
	uint16_t r;

	if (TFT_WCTRL == PORTC || TFT_WDATA == PORTC) {
		MCUCR |= 0x80;			// Disable JTAG
		MCUCR |= 0x80;
	}

	TFT_PCTRL = 0xFF & ~TFT_FMK;
	TFT_WCTRL = 0xFF & ~TFT_BLC;	// Disable background light
	mode(true);			// Read mode

	TFT_WCTRL &= ~TFT_RST;	// Hardware reset
	TFT_WCTRL &= ~TFT_CS;
	TFT_WCTRL |= TFT_WR;
	TFT_WCTRL |= TFT_RD;
	TFT_WCTRL |= TFT_RS;
	TFT_WCTRL |= TFT_VSY;
	_delay_us(10);		// Min: 10us
	TFT_WCTRL |= TFT_RST;
	_delay_ms(120);
	mode(false);		// Write mode
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
			data(0x00);
		}
	cmd(0xB1);		// Frame Rate control, normal
	data(0x00);		// Faster
	data(0x18);
	cmd(0xB3);		// Frame Rate control, partial
	data(0x00);		// Faster
	data(0x18);
	cmd(0x29);		// Display On
}

#endif
