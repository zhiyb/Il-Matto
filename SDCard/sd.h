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
	cidReg(void) {}
	cidReg(uint8_t dat[16]);

	uint8_t MID;		// Manufacturer ID
	uint16_t OID;		// OEM/Application ID
	char PNM[5];		// Product name
	uint8_t PRV;		// Product revision
	uint32_t PSN;		// Product serial number
	uint16_t MDT:12;	// Manufacturing date
	uint8_t CRC:7;		// CRC7 checksum
};

class csdReg
{
public:
	csdReg(void) {}
	csdReg(uint8_t dat[16]);

	uint8_t CSD_STRUCTURE:2;
	uint8_t TAAC;
	uint8_t NSAC;
	uint8_t TRAN_SPEED;
	uint16_t CCC:12;
	uint8_t READ_BL_LEN:4;
	uint8_t READ_BL_PARTIAL:1;
	uint8_t WRITE_BLK_MISALIGN:1;
	uint8_t READ_BLK_MISALIGN:1;
	uint8_t DSR_IMP:1;
	uint32_t C_SIZE:22;	// 12 for ver1, 22 for ver2
	uint8_t VDD_R_CURR_MIN:3;
	uint8_t VDD_R_CURR_MAX:3;
	uint8_t VDD_W_CURR_MIN:3;
	uint8_t VDD_W_CURR_MAX:3;
	uint8_t C_SIZE_MULT:3;
	uint8_t ERASE_BLK_EN:1;
	uint8_t SECTOR_SIZE:7;
	uint8_t WP_GRP_SIZE:7;
	uint8_t WP_GRP_ENABLE:1;
	uint8_t R2W_FACTOR:3;
	uint8_t WRITE_BL_LEN:4;
	uint8_t WRITE_BL_PARTIAL:1;
	uint8_t FILE_FORMAT_GRP:1;
	uint8_t COPY:1;
	uint8_t PERM_WRITE_PROTECT:1;
	uint8_t TMP_WRITE_PROTECT:1;
	uint8_t FILE_FORMAT:2;
	uint8_t CRC:7;
};

class sdhw: public spi
{
public:
	sdhw(void);
	virtual uint8_t init(void);
	virtual bool detect(void);
	virtual bool writeProtected(void);
	virtual uint8_t version(void) const {return _ver;}
	virtual struct cidReg& cid(void) {return _cid;}
	virtual struct csdReg& csd(void) {return _csd;}
	virtual uint32_t size(void) const;
	virtual uint8_t readBlock(uint32_t addr, uint8_t buff[512]);

protected:
	virtual void cmd(uint8_t index, uint32_t arg, uint8_t crc);
	virtual void acmd(uint8_t index, uint32_t arg, uint8_t crc);
	virtual uint8_t recv(void);
	virtual uint8_t recv(uint8_t n);
	virtual void wait(void);

public:
	class cidReg _cid;
	class csdReg _csd;
	uint8_t _res[5], _ver;
};

extern class sdhw sd;

#endif
