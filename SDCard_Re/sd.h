#ifndef SD_H
#define SD_H

#define SD_DDR DDRB
#define SD_PORT PORTB
#define SD_PIN PINB

#define SD_CD (1 << 1)
#define SD_WP (1 << 0)

#define GO_IDLE_STATE			0
#define GO_IDLE_STATE_CRC		0x4A
#define SEND_OP_COND			1
#define SEND_IF_COND			8
#define SEND_IF_COND_ARG		0x000001AA
#define SEND_IF_COND_CRC		0x43
#define SEND_CSD			9
#define SEND_CID			10
#define	STOP_TRANSMISSION		12
#define	SET_BLOCKLEN			16
#define READ_SINGLE_BLOCK		17
#define READ_MULTIPLE_BLOCK		18
#define SET_BLOCK_COUNT			23
#define WRITE_SINGLE_BLOCK		24
#define WRITE_MULTIPLE_BLOCK		25
#define APP_CMD				55
#define READ_OCR			58

#define SET_WR_BLOCK_ERASE_COUNT	23
#define APP_SEND_OP_COND		41

#include <avr/io.h>
#include "spi.h"

struct reg {
	uint8_t data[16];
};

class sdhw
{
public:
	enum Operations {Read = 0, Write = 1, Single = 0, Multi = 1};

	sdhw(void);
	uint8_t init(void);
	bool detect(void);
	inline bool writeProtected(void) {return SD_PIN & SD_WP;}
	inline uint8_t err(void) {return errno;}
	inline bool dataInit(const bool rw, const bool multi, const uint32_t addr);
	inline bool readInit(void);
	inline bool dataStop(const bool rw);
	inline struct reg readRegister(const uint8_t type);
	inline uint32_t size(void) {return _size;}
	inline uint32_t getSize(void);

protected:
	static inline void wait(void) {while (spi::trans() != 0xFF);}
	static inline uint8_t response(void);
	static inline uint8_t free(const uint8_t errno);
	static inline uint8_t cmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);
	static inline uint8_t acmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);

	uint32_t _size;
	uint8_t errno;
};

extern class sdhw sd;

inline bool sdhw::dataInit(const bool rw, const bool multi, const uint32_t addr)
{
	uint8_t command;
	if (rw)
		command = WRITE_SINGLE_BLOCK;
	else
		command = READ_SINGLE_BLOCK;
	if (multi)
		command++;
	if ((errno = cmd(command, addr)) != 0x00)
		return false;
	if (command == WRITE_SINGLE_BLOCK || command == WRITE_MULTIPLE_BLOCK)
		spi::trans();
	return true;
}

inline bool sdhw::readInit(void)
{
	if ((errno = response()) != 0xFE)
		return false;
	return true;
}

inline bool sdhw::dataStop(const bool rw)
{
	cmd(STOP_TRANSMISSION);
	if (rw == Read && (errno = response()) != 0x00)
		return false;
	wait();
	return true;
}

inline uint8_t sdhw::free(const uint8_t errno)
{
	spi::free(true);
	return errno;
}

inline uint8_t sdhw::response(void)
{
	uint8_t res;
	while ((res = spi::trans()) == 0xFF);
	return res;
}

inline uint8_t sdhw::cmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	wait();
	spi::trans(index | _BV(6));
	spi::trans32(arg);
	spi::trans((crc << 1) | 1);
	return response();
}

inline uint8_t sdhw::acmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	uint8_t res = cmd(APP_CMD);
	if (res > 0x01)
		return res;
	return cmd(index, arg, crc);
}

inline struct reg sdhw::readRegister(const uint8_t type)
{
	struct reg r;
	spi::free(false);
	spi::assert(true);
	spi::trans();
	if ((errno = cmd(type)) != 0x00)
		goto ret;
	if ((errno = response()) != 0xFE)
		goto ret;
	for (uint8_t i = 0; i < 16; i++)
		r.data[i] = spi::trans();
	spi::trans();
	spi::trans();
	errno = 0;
ret:
	spi::free(true);
	return r;
}

inline uint32_t sdhw::getSize(void)
{
	struct reg csd = readRegister(SEND_CSD);
	if (errno)
		return 0;
	else
		return (((uint32_t)csd.data[7] << 16) | ((uint32_t)csd.data[8] << 8) | csd.data[9]) * 512;
}

#endif
