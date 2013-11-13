#include "spi.h"

void spi::init(void)
{
	DDRB = (1 << 4) | (1 << 5) | (1 << 7);
	// F_SCK = F_CPU / 16 = 750 kHz
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi::send(uint8_t d)
{
	SPDR = d;
	while(!(SPSR & (1 << SPIF)));
}

uint8_t spi::recv(void)
{
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}
