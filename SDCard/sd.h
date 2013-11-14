#ifndef MMC_H
#define MMC_H

#define SD_DDR DDRB
#define SD_PORT PORTB
#define SD_PIN PINB

#define SD_CD (1 << 1)
#define SD_WP (1 << 0)

#include "spi.h"

class cidReg
{
public:
	cidReg(uint8_t dat[16]);

	uint8_t mid;		// Manufacturer ID
	uint16_t oid;		// OEM/Application ID
	char pnm[6];		// Product name
	uint8_t prv;		// Product revision
	uint32_t psn;		// Product serial number
	uint16_t mdt:12;	// Manufacturing date
	uint8_t crc:7;		// CRC7 checksum
};

class csdReg
{
public:
	csdReg(uint8_t dat[16]);
};

class sdhw: public spi
{
public:
	sdhw(void);
	virtual uint8_t init(void);
	virtual bool detect(void);
	virtual bool writeProtected(void);
	virtual uint8_t version(void) const {return ver;}
	virtual struct cidReg cid(void);
	virtual struct csdReg csd(void);

protected:
	virtual void cmd(uint8_t index, uint32_t arg, uint8_t crc);
	virtual void acmd(uint8_t index, uint32_t arg, uint8_t crc);
	virtual uint8_t recv(void);
	virtual uint8_t recv(uint8_t n);
	virtual void wait(void);

	uint8_t res[5], ver;
};

extern class sdhw sd;

#endif
