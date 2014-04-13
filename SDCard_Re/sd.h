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
#include "mbr.h"
#include "hw.h"

struct reg_t {
	uint8_t data[16];
};

class sdhw_t : public hw_t
{
public:
	enum Operations {Single = 0, Multi = 1};

	sdhw_t(void);
	uint8_t init(void);
	bool detect(void);
	uint32_t getSize(void);
	uint8_t getMBR(void);
	inline bool writeProtected(void) {return SD_PIN & SD_WP;}
	inline uint8_t err(void) {return errno;}
	inline bool dataInit(const bool rw, const bool multi, const uint32_t addr);
	inline bool readInit(void);
	inline bool dataStop(const bool rw);
	inline struct reg_t readRegister(const uint8_t type);
	inline uint32_t size(void) {return _size;}
	inline class mbr_t& mbr(void) {return _mbr;}

	virtual inline uint8_t readNextByte(void) {return spi::trans();}
	virtual inline bool dataAddress(const bool rw, const uint32_t addr);

protected:
	static inline void wait(void) {while (spi::trans() != 0xFF);}
	static inline uint8_t response(void);
	static inline uint8_t free(const uint8_t err);
	static inline uint8_t cmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);
	static inline uint8_t acmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);

	class mbr_t _mbr;
	uint32_t _size;
	uint8_t errno;
};

extern class sdhw_t sd;

inline bool sdhw_t::dataInit(const bool rw, const bool multi, const uint32_t addr)
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

inline bool sdhw_t::readInit(void)
{
	if ((errno = response()) != 0xFE)
		return false;
	return true;
}

inline bool sdhw_t::dataStop(const bool rw)
{
	cmd(STOP_TRANSMISSION);
	if (rw == Read && (errno = response()) != 0x00)
		return false;
	wait();
	return true;
}

inline uint8_t sdhw_t::free(const uint8_t err)
{
	spi::free(true);
	spi::assert(false);
	return err;
}

inline uint8_t sdhw_t::response(void)
{
	uint8_t res;
	while ((res = spi::trans()) == 0xFF);
	return res;
}

inline uint8_t sdhw_t::cmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	wait();
	spi::trans(index | _BV(6));
	spi::trans32(arg);
	spi::trans((crc << 1) | 1);
	return response();
}

inline uint8_t sdhw_t::acmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	uint8_t res = cmd(APP_CMD);
	if (res > 0x01)
		return res;
	return cmd(index, arg, crc);
}

inline struct reg_t sdhw_t::readRegister(const uint8_t type)
{
	struct reg_t r;
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

inline bool sdhw_t::dataAddress(const bool rw, const uint32_t addr)
{
	if (!dataInit(rw, Single, addr))
		return false;
	if (rw == Read)
		if (!readInit())
			return false;
	return true;
}

#endif
