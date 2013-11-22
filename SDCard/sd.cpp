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

#include <stdio.h>
uint8_t sdhw::init(void)
{
	_ver = 0;
	_delay_ms(1);				// Wait for 1 ms
	SPI_PORT |= SPI_SS;
	for (uint8_t i = 0; i < 10; i++)	// Apply 74(80) clock pulses
		spi::send(0xFF);
	SPI_PORT &= ~SPI_SS;
	puts("STOPED?");
	cmd(0, 0, 0x4A);			// GO_IDLE_STATE / SW Reset
	puts("JUMP OVER!");
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
	// F_SCK = F_CPU / 2 = 6 MHz		// Fast SPI speed
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR |= 1 << SPI2X;
	uint8_t res;
	uint8_t dat[16];
/*	cmd(10, 0, 0xFF);			// Send card identification
	if ((res = recv(2)) != 0)
		return 0;
	for (res = 0; res < 16; res++)
		dat[res] = recv(1);
	_cid = cidReg(dat);
*/	cmd(9, 0, 0xFF);			// Send card-specific data
	if ((res = recv(2)) != 0)
		return 0;
	for (res = 0; res < 16; res++)
		dat[res] = recv(1);
	_csd = csdReg(dat);
	return 0;
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
