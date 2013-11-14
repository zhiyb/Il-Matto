#include <avr/io.h>
#include <util/delay.h>
#include "sd.h"

class sdhw sd;

csdReg::csdReg(uint8_t dat[16])
{
}

cidReg::cidReg(uint8_t dat[16])
{
	mid = dat[0];
	oid = dat[2] * 0x0100 + dat[1];
	for (uint8_t i = 0; i < 5; i++)
		pnm[i] = dat[3 + i];
	pnm[5] = '\0';
	prv = dat[8];
	psn = 0;
	for (uint8_t i = 0; i < 4; i++) {
		psn <<= 8;
		psn |= dat[9 + i];
	}
	mdt = (dat[13] & 0x0F) * 0x0100 + dat[14];
	crc = dat[15] >> 1;
}

void sdhw::acmd(uint8_t index, uint32_t arg, uint8_t crc)
{
	cmd(55, 0, 0xFF);			// Lead cmd(55)
	cmd(index, arg, crc);
}

void sdhw::cmd(uint8_t index, uint32_t arg, uint8_t crc)
{
	wait();
	wait();
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
	cmd(0, 0, 0x4A);			// GO_IDLE_STATE / SW Reset
	if (recv() != 0x01)
		return 0x01;			// Failed
	cmd(8, 0x000001AA, 0x43);		// CHK voltage range & version
	if (recv() != 0x01)
		goto notver2;			// Not support, not ver2
	if (res[3] != 0x01 || res[4] != 0xAA)
		return 0x02;			// Not match, failed
wakeup:
	acmd(41, 0x40000000, 0xFF);		// Initiate initialisation
	if (recv() == 0x01)
		goto wakeup;			// Retry wakeup
	cmd(58, 0, 0xFF);			// Read OCR
	recv();
	if (res[1] & 0x40) {
		ver = 0x12;
		return 0;
	}
	ver = 2;
	goto bsize;
notver2:
	acmd(41, 0, 0xFF);			// Initiate initialisation
	if (recv() & ~0x01)
		goto notver1;			// Not support
	if (res[0] == 0x01)
		goto notver2;			// Retry wakeup
	ver = 1;
	goto bsize;
notver1:
	cmd(1, 0, 0xFF);			// Initiate initialisation
	if (recv() & ~0x01)
		return 0xFF;			// Not support or error
	if (res[0] == 0x01)
		goto notver1;			// Retry wakeup
	ver = 3;
bsize:
	cmd(16, 0x00000200, 0xFF);		// Force block size 512 bytes
	return 0;
}

struct csdReg sdhw::csd(void)
{
	return 0;
}

struct cidReg sdhw::cid(void)
{
	cmd(10, 0, 0xFF);			// Send card identification
	uint8_t res;
	if ((res = recv(2)) != 0)
		return 0;
	uint8_t dat[16];
	for (res = 0; res < 16; res++)
		dat[res] = recv(1);
	return cidReg(dat);
}

void sdhw::wait(void)
{
	do
		spi::send(0xFF);
	while (spi::recv() != 0xFF);
}

uint8_t sdhw::recv(uint8_t n)
{
	for (uint8_t r = 0; r < n; r++) {
		uint8_t i = 50;
		do {
			spi::send(0xFF);
			i--;
		} while ((res[r] = spi::recv()) == 0xFF && i != 0);
	}
	return res[0];
}

uint8_t sdhw::recv(void)
{
	for (uint8_t r = 0; r < 5; r++) {
		uint8_t i = 50;
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
	if (SD_PIN & SD_CD) {
		ver = 0xFF;		// Indicate new card need to init
		return false;
	}
	_delay_ms(500);
loop:
	wp = writeProtected();
	_delay_ms(100);
	if (writeProtected() != wp)
		goto loop;
	return true;
}

bool sdhw::writeProtected(void)
{
	return SD_PIN & SD_WP;
}
