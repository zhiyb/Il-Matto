#include <avr/io.h>
#include "spi.h"

void spi::init(void)
{
	DDRB |= SPI_SS | SPI_MOSI | SPI_SCK;
	DDRB &= ~SPI_MISO;
	PORTB &= ~(SPI_SS | SPI_MOSI | SPI_MISO | SPI_SCK);
	// F_SCK = F_CPU / 64 = 187.5 kHz
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR &= ~(1 << SPI2X);
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
