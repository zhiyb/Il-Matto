#ifndef LIBTFT_CPP_CONNECTION_H
#define LIBTFT_CPP_CONNECTION_H

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

#endif
