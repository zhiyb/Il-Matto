#include <stdint.h>
#include <avr/io.h>
#include <macros.h>
#include "tlc5940.h"

#define _DDR	CONCAT_E(DDR, TLC5940_PORT)
#define _PORT	CONCAT_E(PORT, TLC5940_PORT)
#define _PIN	CONCAT_E(PIN, TLC5940_PORT)

#define spi_ready()	(UCSR1A & _BV(TXC1))

static inline uint8_t spi_transfer(const uint8_t data)
{
#if 0
	UDR1 = data;
	while (!(UCSR1A & _BV(UDRE1)));
	return 0;
#else
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = data;
	return 0;
#endif
}

void tlc5940_init()
{
	_DDR |= TLC5940_SIN | TLC5940_SCLK | TLC5940_XLAT | TLC5940_GSCLK | TLC5940_DCPRG | TLC5940_BLANK | TLC5940_VPRG;
	_DDR &= ~(TLC5940_XERR);
	_PORT |= TLC5940_SIN | TLC5940_BLANK | TLC5940_XERR;
	_PORT &= ~(TLC5940_SCLK | TLC5940_XLAT | TLC5940_GSCLK | TLC5940_DCPRG | TLC5940_VPRG);

	// Initialise SPI
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
	UCSR1C = _BV(UMSEL11) | _BV(UMSEL10);// | _BV(UCPHA1) | _BV(UCPOL1);
	UBRR1 = 0;

	// Initialise timer (GSCLK)
	TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);	// Fast PWM, top OCRxA
	TCCR2B = _BV(WGM22);
	OCR2A = 1;
	OCR2B = 0;
	TCCR2B |= _BV(CS20);	// Start timer

	// Initialise timer (BLANK)
	TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
	TCCR1B = _BV(WGM13) | _BV(WGM12);
	OCR1A = 8;	// * 1024
	ICR1 = 9;
	TCCR1B |= _BV(CS12) | _BV(CS10);	// Start timer

	// DC data
	uint8_t i = 96 / 8;
	while (i--)
		spi_transfer(0xff);
	while (!spi_ready());
	_PORT |= TLC5940_XLAT;
	_PORT &= ~TLC5940_XLAT;
}

void tlc5940_blank(const uint8_t blank)
{
	if (blank)
		_PORT |= TLC5940_BLANK;
	else
		_PORT &= ~TLC5940_BLANK;
}

void tlc5940_set_gs(uint8_t count, uint8_t offset, const void *ptr)
{
	//uint8_t pulse = 0;
	if (_PORT & TLC5940_VPRG) {
		_PORT &= ~TLC5940_VPRG;
	//	pulse = 1;
	} else {
#if 0
		UCSR1B = _BV(RXEN1);
		_PORT |= TLC5940_SCLK;
		_PORT &= ~TLC5940_SCLK;
		UCSR1B = _BV(RXEN1) | _BV(TXEN1);
#endif
	}
#if 0
	count = 24;
	while (count--)
		spi_transfer(0x11);
#else
	const uint8_t *p = ptr;
	uint8_t status = 0;
	while (count) {
		uint8_t data = 0;
		if (offset == 1) {
			switch (status) {
			case 0:
				data = *p;
				status = 1;
				break;
			case 1:
				data = (*p & 0x01) ? 0xf0 : 0x00;
				if (--count)
					data |= *++p >> 4;
				status = 2;
				break;
			case 2:
				data = (*p << 4) | ((*p & 0x01) ? 0x0f : 0x00);
				p++;
				count--;
				status = 0;
			}
		} else {
			break;
		}
		spi_transfer(data);
	}
#endif
	while (!spi_ready());
#if 0
	UCSR1B = _BV(RXEN1);
	_PORT |= TLC5940_SCLK;
	_PORT &= ~TLC5940_SCLK;
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
#endif
	//_PORT |= TLC5940_BLANK;
	//TCCR1A = _BV(WGM11);
	_PORT |= TLC5940_XLAT;
	_PORT &= ~TLC5940_XLAT;
	//TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(WGM11);
	//_PORT &= ~TLC5940_BLANK;
	//_PORT |= TLC5940_VPRG;
	//if (pulse) {
	//}
#if 0
	UCSR1B = _BV(RXEN1);
	_PORT |= TLC5940_SCLK;
	_PORT &= ~TLC5940_SCLK;
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);
#endif
}
