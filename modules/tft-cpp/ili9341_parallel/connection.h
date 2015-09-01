/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_CONNECTION_H
#define TFT_CONNECTION_H

// 8-bit parallel interface I

#ifndef TFT_PORT_CTRL
#define TFT_PORT_CTRL	A
#endif
#ifndef TFT_PORT_DATA
#define TFT_PORT_DATA	C
#endif

#define TFT_CS	(1 << 0)	// Chip-select (0)
#define TFT_BLC	(1 << 1)	// Background light control (0)
#define TFT_RST	(1 << 2)	// Reset (0)
#define TFT_WR	(1 << 3)	// Parallel data write strobe (Rising)
#define TFT_RS	(1 << 4)	// Data(1) / Command(0) selection
#define TFT_RD	(1 << 5)	// Parallel data read strobe (Rising)
#define TFT_VSY	(1 << 6)	// VSYNC
#define TFT_FMK	(1 << 7)	// Frame mark

#endif
