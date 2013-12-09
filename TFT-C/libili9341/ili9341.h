#ifndef ILI9341_H
#define ILI9341_H

// 8-bit parallel interface I

#define TFT_PCTRL	DDRA
#define TFT_WCTRL	PORTA
#define TFT_RCTRL	PINA
#define TFT_PDATA	DDRC
#define TFT_WDATA	PORTC
#define TFT_RDATA	PINC

#define TFT_CS	(1 << 0)	// Chip-select (0)
#define TFT_BLC	(1 << 1)	// Background light control (0)
#define TFT_RST	(1 << 2)	// Reset (0)
#define TFT_WR	(1 << 3)	// Parallel data write strobe (Rising)
#define TFT_RS	(1 << 4)	// Data(1) / Command(0) selection
#define TFT_RD	(1 << 5)	// Parallel data read strobe (Rising)
#define TFT_VSY	(1 << 6)	// VSYNC
#define TFT_FMK	(1 << 7)	// Frame mark

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>

enum Orientation {Landscape, Portrait, FlipLandscape, FlipPortrait, \
	BMPLandscape, BMPPortrait, BMPFlipLandscape, BMPFlipPortrait};

// Defined as inline to excute faster

static inline void ili9341_init(void);

static inline void ili9341_mode(uint8_t _recv);
static inline void ili9341_send(uint8_t c, uint8_t dat);
static inline void ili9341_cmd(uint8_t dat);
static inline void ili9341_data(uint8_t dat);
static inline uint8_t ili9341_recv(void);

static inline void ili9341_setBGLight(uint8_t ctrl);
static inline void ili9341_setOrient(uint8_t o);
static inline void ili9341_idle(uint8_t e);
static inline void ili9341_sleep(uint8_t e);
static inline void ili9341_inversion(uint8_t e);

#define LOW(b)	TFT_WCTRL &= ~(b)
#define HIGH(b)	TFT_WCTRL |= (b)
#define SEND() TFT_PDATA = 0xFF
#define RECV() do { \
	TFT_PDATA = 0x00; \
	TFT_WDATA = 0xFF; \
} while(0)

static inline void ili9341_cmd(uint8_t dat)
{
	LOW(TFT_RS);
	TFT_WDATA = dat;
	LOW(TFT_WR);
	HIGH(TFT_WR);
	HIGH(TFT_RS);
}

static inline void ili9341_data(uint8_t dat)
{
	TFT_WDATA = dat;
	LOW(TFT_WR);
	HIGH(TFT_WR);
}

static inline void ili9341_idle(uint8_t e)
{
	ili9341_cmd(0x38 + e);
}

static inline void ili9341_sleep(uint8_t e)
{
	ili9341_cmd(0x10 + e);
}

static inline void ili9341_inversion(uint8_t e)
{
	ili9341_cmd(0x20 + e);
}

static inline void ili9341_setOrient(uint8_t o)
{
	ili9341_cmd(0x36);		// Memory Access Control
	switch (o) {
	case Landscape:
		ili9341_data(0x28);	// Column Address Order, BGR
		break;
	case Portrait:
		ili9341_data(0x48);	// Column Address Order, BGR
		break;
	case FlipLandscape:
		ili9341_data(0xE8);		// Column Address Order, BGR
		break;
	case FlipPortrait:
		ili9341_data(0x88);		// Column Address Order, BGR
		break;
	case BMPLandscape:
		ili9341_data(0x68);		// Column Address Order, BGR
		break;
	case BMPFlipLandscape:
		ili9341_data(0xA8);		// Column Address Order, BGR
		break;
	case BMPPortrait:
		ili9341_data(0xD8);		// Column Address Order, BGR
		break;
	case BMPFlipPortrait:
		ili9341_data(0x18);		// Column Address Order, BGR
		break;
	}
}

static inline void ili9341_mode(uint8_t _recv)
{
	if (_recv)
		RECV();
	else
		SEND();
}

static inline void ili9341_send(uint8_t c, uint8_t dat)
{
	if (c)
		ili9341_cmd(dat);
	else
		ili9341_data(dat);
}

static inline uint8_t ili9341_recv(void)
{
	unsigned char dat;
	LOW(TFT_RD);
	_NOP();
	dat = TFT_RDATA;
	HIGH(TFT_RD);
	return dat;
}

static inline void ili9341_setBGLight(uint8_t ctrl)
{
	if (ctrl)
		TFT_WCTRL |= TFT_BLC;
	else
		TFT_WCTRL &= ~TFT_BLC;
}

static inline void ili9341_init(void)
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
	SEND();
	ili9341_cmd(0x28);		// Display OFF
	ili9341_cmd(0x11);		// Sleep Out
	_delay_ms(120);
	ili9341_cmd(0x34);		// Tearing Effect Line OFF
	ili9341_cmd(0x38);		// Idle Mode OFF
	ili9341_cmd(0x13);		// Normal Display Mode ON
	ili9341_cmd(0x20);		// Display Inversion OFF
	ili9341_cmd(0x3A);		// COLMOD: Pixel Format Set
	ili9341_data(0x55);		// 16 bits/pixel
	ili9341_cmd(0x36);		// Memory Access Control
	ili9341_data(0x48);		// Column Adress Order, BGR
	ili9341_cmd(0x2C);		// Memory Write
	for (r = 0; r < 320; r++)	// Black screen
		for (c = 0; c < 240; c++) {
			ili9341_data(0x00);
			ili9341_data(0x00);
			ili9341_data(0x00);
		}
	ili9341_cmd(0xB1);		// Frame Rate control, normal
	ili9341_data(0x00);		// 70Hz
	ili9341_data(0x1B);
	ili9341_cmd(0xB3);		// Frame Rate control, partial
	ili9341_data(0x00);		// 70Hz
	ili9341_data(0x1B);
	ili9341_cmd(0x29);		// Display On
}

#undef LOW
#undef HIGH
#undef SEND
#undef RECV

#endif
