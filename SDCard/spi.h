#ifndef SPI_H
#define SPI_H

#define SPI_PORT PORTB
#define SPI_PIN PINB
#define SPI_DDR DDRB
#define SPI_SS (1 << 4)
#define SPI_MOSI (1 << 5)
#define SPI_MISO (1 << 6)
#define SPI_SCK (1 << 7)

#include <avr/io.h>
#include <inttypes.h>

class spi
{
protected:
	static inline void init(void);
	static inline void send(uint8_t d);
	static inline uint8_t recv(void);
};

// Defined as inline to execute faster

inline void spi::init(void)
{
	DDRB |= SPI_SS | SPI_MOSI | SPI_SCK;
	DDRB &= ~SPI_MISO;
	PORTB |= SPI_SS | SPI_MOSI | SPI_MISO | SPI_SCK;
	// F_SCK = F_CPU / 64 = 187.5 kHz
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR &= ~(1 << SPI2X);
}

inline void spi::send(uint8_t d)
{
	SPDR = d;
	while(!(SPSR & (1 << SPIF)));
}

inline uint8_t spi::recv(void)
{
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

#endif
