#ifndef SPI_H
#define SPI_H

#define SPI_PORT PORTB
#define SPI_SS (1 << 4)
#define SPI_MOSI (1 << 5)
#define SPI_MISO (1 << 6)
#define SPI_SCK (1 << 7)

#include <inttypes.h>

class spi
{
protected:
public:
	static void init(void);
	static void send(uint8_t d);
	static uint8_t recv(void);
};

#endif
