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
	static inline void slow(void);
	static inline void fast(void);
	static inline uint8_t trans(void);
	static inline uint8_t trans(uint8_t d);
	static inline uint8_t trans8(void) {return trans();}
	static inline uint16_t trans16(void);
	static inline uint32_t trans24(void);
	static inline uint32_t trans32(void);
};

// Defined as inline to execute faster

inline void spi::init(void)
{
	DDRB |= SPI_SS | SPI_MOSI | SPI_SCK;
	DDRB &= ~SPI_MISO;
	PORTB |= SPI_SS | SPI_MOSI | SPI_MISO | SPI_SCK;
}

inline void spi::slow(void)
{
	// F_SCK = F_CPU / 32 = 375 kHz
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);
	SPSR |= (1 << SPI2X);
}

inline void spi::fast(void)
{
	// F_SCK = F_CPU / 2 = 6 MHz		// Fast SPI speed
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR |= (1 << SPI2X);
}

inline uint8_t spi::trans(void)
{
	SPDR = 0xFF;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

inline uint8_t spi::trans(uint8_t d)
{
	SPDR = d;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}

inline uint16_t spi::trans16(void)
{
	return (uint32_t)spi::trans() | \
		((uint32_t)spi::trans() * 0x0100);
}

inline uint32_t spi::trans24(void)
{
	return (uint32_t)spi::trans() | \
		((uint32_t)spi::trans() * 0x0100) | \
		((uint32_t)spi::trans() * 0x00010000);
}

inline uint32_t spi::trans32(void)
{
	return (uint32_t)spi::trans() | \
		((uint32_t)spi::trans() * 0x0100) | \
		((uint32_t)spi::trans() * 0x00010000) | \
		((uint32_t)spi::trans() * 0x01000000);
}

#endif
