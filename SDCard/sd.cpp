#include <avr/io.h>
#include <util/delay.h>
#include "sd.h"

class sdhw sd;

void sdhw::send(uint8_t index, uint32_t arg, uint8_t crc)
{
	spi::send(index | (1 << 6));
	spi::send(arg >> 24);
	spi::send(arg >> 16);
	spi::send(arg >> 8);
	spi::send(arg);
	spi::send((crc << 1) | 1);
}

sdhw::sdhw(void)
{
	spi::init();
	SD_DDR &= ~(SD_CD | SD_WP);
	SD_PORT |= SD_CD | SD_WP;
}

uint8_t sdhw::init(void)
{
	ver = 0;
	_delay_ms(1);				// Wait for 1 ms
	SPI_PORT |= SPI_SS;
	for (uint8_t i = 0; i < 10; i++)	// Apply 74(80) clock pulses
		spi::send(0xFF);
	SPI_PORT &= ~SPI_SS;
	send(0, 0, 0x4A);			// GO_IDLE_STATE / SW Reset
	if (recv() != 0x01)
		return 0x01;			// Failed
	send(8, 0x000001AA, 0x43);		// CHK voltage range & version
	if (recv() != 0x01)
		goto notver2;			// Not support, not ver2
	if (res[3] != 0x01 || res[4] != 0xAA)
		return 0x02;			// Not match, failed
wakeup:
	send(55, 0, 0xFF);			// ACMD<41>
	recv();
	send(41, 0x40000000, 0xFF);		// Initiate initialization
	if (recv() == 0x01)
		goto wakeup;
	return 0x00;
notver2:
	return 0xFF;
}

uint8_t sdhw::recv(void)
{
	for (uint8_t r = 0; r < 5; r++) {
		uint8_t i = 255;
		do {
			spi::send(0xFF);
			i--;
		} while ((res[r] = spi::recv()) == 0xFF && i != 0);
	}
	return res[0];
}

bool sdhw::detect(void)
{
	bool wp;
	while (SD_PIN & SD_CD);
	_delay_ms(500);
loop:
	wp = writeProtected();
	_delay_ms(100);
	if (writeProtected() != wp)
		goto loop;
	return !(SD_PIN & SD_CD);
}

bool sdhw::writeProtected(void)
{
	return SD_PIN & SD_WP;
}
