/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef ILI9341_H
#define ILI9341_H

#include "connection.h"

#include <avr/io.h>
#include <util/delay.h>
#include <macros.h>

#define TFT_SIZE_HEIGHT	320
#define TFT_SIZE_WIDTH	240
#define TFT_DEF_ORIENT	tft::Portrait

#define TFT_PCTRL	CONCAT_E(DDR, TFT_PORT_CTRL)
#define TFT_WCTRL	CONCAT_E(PORT, TFT_PORT_CTRL)
#define TFT_RCTRL	CONCAT_E(PIN, TFT_PORT_CTRL)
#define TFT_PDATA	CONCAT_E(DDR, TFT_PORT_DATA)
#define TFT_WDATA	CONCAT_E(PORT, TFT_PORT_DATA)
#define TFT_RDATA	CONCAT_E(PIN, TFT_PORT_DATA)

namespace tft
{
	enum Orientation {Landscape = 0, Portrait, \
		FlipLandscape, FlipPortrait, \
		BMPLandscape, BMPPortrait, \
		BMPFlipLandscape, BMPFlipPortrait};
}

namespace tfthw
{
	static inline void cmd(uint8_t dat);
	static inline void data(uint8_t dat);
	//static inline void send(bool c, uint8_t dat);
	static inline void setBGLight(bool ctrl);
	static inline void setOrient(uint8_t o);
	
	static inline void idle(bool e) {cmd(0x38 + e);}
	static inline void sleep(bool e) {cmd(0x10 + e);}
	static inline void inversion(bool e) {cmd(0x20 + e);}
};

// Port
namespace tfthw
{
	static inline void init();
	static inline void setPage(const uint16_t start, const uint16_t end);
	static inline void setColumn(const uint16_t start, const uint16_t end);
	// 0x2c Write, 0x2e Read, 0x3c / 0x3e Continue, 0x00 NOP
	static inline void memWrite() {cmd(0x2c);}
	static inline void memRead() {cmd(0x2e);}
	static inline void mode(bool read);
	static inline void write(const uint8_t d) {data(d);}
	static inline void write16(const uint16_t c) {write(c >> 8); write(c & 0xff);}
	static inline uint8_t read();
}

// Defined as inline to excute faster

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
	// Landscape, Portrait, FlipLandscape, FlipPortrait
	// BMPLandscape, BMPPortrait, BMPFlipLandscape, BMPFlipPortrait
	static const uint8_t orient[] = {
			0x28, 0x48, 0xE8, 0x88,
			0x68, 0xD8, 0xA8, 0x18
	};
	cmd(0x36);			// Memory Access Control
	data(orient[o]);
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

/*static inline void tfthw::send(bool c, uint8_t dat)
{
	if (c)
		cmd(dat);
	else
		data(dat);
}*/

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
