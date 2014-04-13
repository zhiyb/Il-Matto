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

namespace spi
{
	static inline void slow(void);
	static inline void fast(void);
	static inline void free(const bool e);
	static inline void init(void) {spi::free(false);}
	static inline void assert(const bool e);
	static inline uint8_t trans(const uint8_t d = 0xFF);
	static inline uint8_t trans8(void) {return trans();}
	static inline uint16_t trans16(void);
	static inline uint16_t trans16big(void);
	static inline uint32_t trans24(void);
	static inline uint32_t trans32(const uint32_t d = 0xFFFFFFFF);
	static inline uint32_t trans32big(const uint32_t d = 0xFFFFFFFF);
};

// Defined as inline to execute faster

inline void spi::free(const bool e)
{
	if (e) {
		DDRB &= ~(SPI_MOSI | SPI_SCK | SPI_MISO);
		PORTB &= ~(SPI_MOSI | SPI_SCK | SPI_MISO);
	} else {
		DDRB |= SPI_SS | SPI_MOSI | SPI_SCK;
		DDRB &= ~SPI_MISO;
		PORTB |= SPI_SS | SPI_MOSI | SPI_MISO | SPI_SCK;
	}
}

inline void spi::assert(const bool e)
{
	if (e)
		SPI_PORT &= ~SPI_SS;
	else
		SPI_PORT |= SPI_SS;
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

inline uint8_t spi::trans(const uint8_t d)
{
	SPDR = d;
	while(!(SPSR & _BV(SPIF)));
	return SPDR;
}

inline uint16_t spi::trans16(void)
{
	uint16_t res = spi::trans();
	res |= (uint16_t)spi::trans() << 8;
	return res;
}

inline uint16_t spi::trans16big(void)
{
	uint16_t res = (uint16_t)spi::trans() << 8;
	res |= (uint16_t)spi::trans();
	return res;
}

inline uint32_t spi::trans24(void)
{
	uint32_t res = spi::trans();
	res |= (uint32_t)spi::trans() << 8;
	res |= (uint32_t)spi::trans() << 16;
	return res;
}

inline uint32_t spi::trans32(const uint32_t d)
{
	uint32_t res = spi::trans((d >> 24) & 0xFF);
	res |= (uint32_t)spi::trans((d >> 16) & 0xFF) << 8;
	res |= (uint32_t)spi::trans((d >> 8) & 0xFF) << 16;
	res |= (uint32_t)spi::trans(d & 0xFF) << 24;
	return res;
}

inline uint32_t spi::trans32big(const uint32_t d)
{
	uint32_t res = (uint32_t)spi::trans((d >> 24) & 0xFF) << 24;
	res |= (uint32_t)spi::trans((d >> 16) & 0xFF) << 16;
	res |= (uint32_t)spi::trans((d >> 8) & 0xFF) << 8;
	res |= (uint32_t)spi::trans(d & 0xFF);
	return res;
}

#endif
