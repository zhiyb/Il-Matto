/*
 * Author: Yubo Zhi (normanzyb@gmail.com)
 */

#ifndef TFT_CONNECTION_H
#define TFT_CONNECTION_H

// 4 wire serial interface, without read line

// Port D only, currently
#undef TFT_PORT
#define TFT_PORT	D

// Using USART1 on PORTD as SPI
#define TFT_LED		(1U << 2)
//#define TFT_MISO	(1U << 2)	// Not connected
#define TFT_MOSI	(1U << 3)
#define TFT_SCK		(1U << 4)
#define TFT_CS		(1U << 5)
#define TFT_DCX		(1U << 6)
#define TFT_RST		(1U << 7)

#endif
