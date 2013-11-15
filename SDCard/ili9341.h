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
#include <util/delay.h>

class ili9341
{
public:
	enum Orientation {Landscape, Portrait, FlipLandscape, FlipPortrait};

	static void init(void);

protected:
	static inline void send(bool cmd, unsigned char dat);
	static inline unsigned char recv(void);
	static inline void _setBGLight(bool ctrl);
	static inline void _setOrient(uint8_t o);
};

// Defined as inline to excute faster

#define LOW(b)	TFT_WCTRL &= ~(b)
#define HIGH(b)	TFT_WCTRL |= (b)
#define SEND() TFT_PDATA = 0xFF
#define RECV() { \
	TFT_PDATA = 0x00; \
	TFT_WDATA = 0xFF; \
}

inline void ili9341::_setOrient(uint8_t o)
{
	send(1, 0x36);			// Memory Access Control
	switch (o) {
	case Landscape:
		send(0, 0x28);		// Column Adress Order, BGR
		break;
	case Portrait:
		send(0, 0x48);		// Column Adress Order, BGR
		break;
	case FlipLandscape:
		send(0, 0xE8);		// Column Adress Order, BGR
		break;
	case FlipPortrait:
		send(0, 0x88);		// Column Adress Order, BGR
	}
}

inline void ili9341::send(bool cmd, unsigned char dat)
{
	SEND();
	if (cmd)
		LOW(TFT_RS);
	TFT_WDATA = dat;
	LOW(TFT_WR);
	HIGH(TFT_WR);
	HIGH(TFT_RS);
}

inline unsigned char ili9341::recv(void)
{
	unsigned char dat;
	RECV();
	LOW(TFT_RD);
	_delay_us(1);
	dat = TFT_RDATA;
	HIGH(TFT_RD);
	return dat;
}

inline void ili9341::_setBGLight(bool ctrl)
{
	if (ctrl)
		TFT_WCTRL |= TFT_BLC;
	else
		TFT_WCTRL &= ~TFT_BLC;
}

#undef LOW
#undef HIGH
#undef SEND
#undef RECV

#endif
