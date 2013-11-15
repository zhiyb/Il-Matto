#include <avr/io.h>
#include <util/delay.h>
#include "sd.h"

class sdhw sd;

csdReg::csdReg(uint8_t dat[16])
{
	CSD_STRUCTURE = dat[0] >> 6;
	TAAC = dat[1];
	NSAC = dat[2];
	TRAN_SPEED = dat[3];
	CCC = (dat[4] << 4) | (dat[5] >> 4);
	READ_BL_LEN = dat[5];
	READ_BL_PARTIAL = dat[6] >> 7;
	WRITE_BLK_MISALIGN = dat[6] >> 6;
	READ_BLK_MISALIGN = dat[6] >> 5;
	DSR_IMP = dat[6] >> 4;
	if (CSD_STRUCTURE == 0) {	// Ver 1
		// Reserved 2 bit
		C_SIZE = ((dat[6] & 0x03) << 10) | \
			 (dat[7] << 2) | (dat[8] >> 6);
		VDD_R_CURR_MIN = dat[8] >> 3;
		VDD_R_CURR_MAX = dat[8];
		VDD_W_CURR_MIN = dat[9] >> 5;
		VDD_W_CURR_MAX = dat[9] >> 2;
		C_SIZE_MULT = (dat[9] << 1) + (dat[10] >> 7);
	} else {			// Ver 2
		// Reserved 6 bit
		C_SIZE = ((uint32_t)dat[7] << 16) | \
			 ((uint32_t)dat[8] << 8) | dat[9];
		// Reserved 1 bit
	}
	ERASE_BLK_EN = dat[10] >> 6;
	SECTOR_SIZE = (dat[10] << 1) | (dat[11] >> 7);
	WP_GRP_SIZE = dat[11];
	WP_GRP_ENABLE = dat[12] >> 7;
	// Reserved 2 bit
	R2W_FACTOR = dat[12] >> 2;
	WRITE_BL_LEN = (dat[12] << 2) | (dat[13] >> 6);
	WRITE_BL_PARTIAL = dat[13] >> 5;
	// Reserved 5 bit
	FILE_FORMAT_GRP = dat[14] >> 7;
	COPY = dat[14] >> 6;
	PERM_WRITE_PROTECT = dat[14] >> 5;
	TMP_WRITE_PROTECT = dat[14] >> 4;
	FILE_FORMAT = dat[14] >> 2;
	// Reserved 2 bit
	CRC = dat[15] >> 1;
}

cidReg::cidReg(uint8_t dat[16])
{
	MID = dat[0];
	OID = dat[2] * 0x0100 + dat[1];
	for (uint8_t i = 0; i < 5; i++)
		        PNM[i] = dat[3 + i];
	PRV = dat[8];
	PSN = 0;
	for (uint8_t i = 0; i < 4; i++) {
		PSN <<= 8;
		PSN |= dat[9 + i];
	}
	MDT = dat[13] * 0x0100 + dat[14];
	CRC = dat[15] >> 1;
	return;
}

uint8_t sdhw::readBlock(uint32_t addr, uint8_t buff[512])
{
	cmd(17, addr, 0xFF);			// Read block
	uint8_t res = recv(1);
	if (res)
		return res;
	do
		spi::send(0xFF);
	while (spi::recv() != 0xFE);
	for (uint16_t i = 0; i < 512; i++) {
		spi::send(0xFF);
		buff[i] = spi::recv();
	}
	wait();
	return 0;
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
	_ver = 0;
}

uint8_t sdhw::init(void)
{
	_ver = 0;
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
	if (_res[3] != 0x01 || _res[4] != 0xAA)
		return 0x02;			// Not match, failed
wakeup:
	acmd(41, 0x40000000, 0xFF);		// Initiate initialisation
	if (recv() == 0x01)
		goto wakeup;			// Retry wakeup
	cmd(58, 0, 0xFF);			// Read OCR
	recv();
	if (_res[1] & 0x40) {
		_ver = 0x12;
		goto info;
	}
	_ver = 2;
	goto bsize;
notver2:
	acmd(41, 0, 0xFF);			// Initiate initialisation
	if (recv() & ~0x01)
		goto notver1;			// Not support
	if (_res[0] == 0x01)
		goto notver2;			// Retry wakeup
	_ver = 1;
	goto bsize;
notver1:
	cmd(1, 0, 0xFF);			// Initiate initialisation
	if (recv() & ~0x01)
		return 0xFF;			// Not support or error
	if (_res[0] == 0x01)
		goto notver1;			// Retry wakeup
	_ver = 3;
bsize:
	cmd(16, 0x00000200, 0xFF);		// Force block size 512 bytes
info:						// Get SD Card info
	uint8_t res;
	uint8_t dat[16];
	cmd(10, 0, 0xFF);			// Send card identification
	if ((res = recv(2)) != 0)
		return 0;
	for (res = 0; res < 16; res++)
		dat[res] = recv(1);
	_cid = cidReg(dat);
	cmd(9, 0, 0xFF);			// Send card-specific data
	if ((res = recv(2)) != 0)
		return 0;
	for (res = 0; res < 16; res++)
		dat[res] = recv(1);
	_csd = csdReg(dat);
	return 0;
}

uint32_t sdhw::size(void) const
{
	if (!_csd.CSD_STRUCTURE)
		return (uint32_t)(_csd.C_SIZE + 1) * \
			(uint32_t)((uint32_t)1 << (_csd.C_SIZE_MULT + 2)) * \
			(uint32_t)((uint32_t)1 << _csd.READ_BL_LEN) / 1024;
	else
		return (uint32_t)(_csd.C_SIZE + 1) * (uint32_t)512;
}

void sdhw::wait(void)
{
	do
		spi::send(0xFF);
	while (spi::recv() != 0xFF);
}

uint8_t sdhw::recv(uint8_t n)
{
	n = n > 5 ? 5 : n;
	for (uint8_t r = 0; r < n; r++) {
		uint8_t i = 100;
		do {
			spi::send(0xFF);
			i--;
		} while (((_res[r] = spi::recv()) == 0xFF) && (i != 0));
	}
	return _res[0];
}

uint8_t sdhw::recv(void)
{
	for (uint8_t r = 0; r < 5; r++) {
		uint8_t i = 100;
		do {
			spi::send(0xFF);
			i--;
		} while ((_res[r] = spi::recv()) == 0xFF && i != 0);
	}
	return _res[0];
}

bool sdhw::detect(void)
{
	bool wp;
	if (SD_PIN & SD_CD) {
		_ver = 0xFF;		// Indicate new card need to init
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
