#ifndef SD_H
#define SD_H

#define SD_DDR DDRB
#define SD_PORT PORTB
#define SD_PIN PINB

#define SD_CD _BV(PB0)
#define SD_WP _BV(PB1)

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
#include "hw.h"

struct reg_t {
	uint8_t data[16];
};

class sdhw_t : public hw_t
{
public:
	enum Operations {Single = 0, Multiple = 1};
	enum ErrorTypes {Succeed = 0, NotExist = 1, GO_IDLE_STATE_Failed = 2, \
			SEND_IF_COND_Failed = 3, SEND_IF_COND_CheckFailed = 4, \
			APP_SEND_OP_COND_Failed = 5, READ_OCR_Failed = 6, CCS_Failed = 7};

	sdhw_t(void);
	uint8_t init(void);
	bool detect(void);
	uint32_t getSize(void);
	struct reg_t readRegister(const uint8_t type);
	inline bool writeProtected(void) {return SD_PIN & SD_WP;}
	inline uint8_t err(void) const {return errno;}
	inline bool dataInit(const bool rw, const bool multi, const uint32_t addr);
	inline bool dataStop(const bool rw, const bool multi);
	inline bool readInit(void);
	inline uint32_t size(void) const {return _size;}
	static inline uint16_t readCRC(void) {return spi::trans16big();}

	virtual inline uint8_t nextByte(void);
	virtual inline bool streamStart(const bool rw, const uint32_t addr);
	virtual inline bool streamStop(const bool rw) {return dataStop(rw, Multiple);}

protected:
	static inline void wait(void) {while (spi::trans() != 0xFF);}
	static inline uint8_t response(void);
	static inline uint8_t free(const uint8_t err = 0);
	static inline void send(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);
	static inline uint8_t cmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);
	static inline uint8_t acmd(const uint8_t index, const uint32_t arg = 0, const uint8_t crc = 0xFF);

	uint32_t _size;
	uint16_t counter;
	uint8_t errno;
};

inline bool sdhw_t::dataInit(const bool rw, const bool multi, const uint32_t addr)
{
	spi::free(false);
	counter = 0;
	uint8_t command;
	if (rw)
		command = WRITE_SINGLE_BLOCK;
	else
		command = READ_SINGLE_BLOCK;
	if (multi)
		command++;
	if ((errno = cmd(command, addr)) != 0x00)
		return free(false);
	return true;
}

inline bool sdhw_t::readInit(void)
{
	if ((errno = response()) != 0xFE)
		return false;
	return true;
}

inline bool sdhw_t::dataStop(const bool rw, const bool multi)
{
	if (multi) {
		send(STOP_TRANSMISSION);
		spi::trans();
		if (rw == Read && (errno = response()) != 0x00)
			return free(false);
		wait();
	} else
		for (uint16_t i = 0; i < 512 + 2; i++)
			spi::trans();
	return free(true);
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

inline void sdhw_t::send(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	spi::assert(false);
	//spi::trans();
	spi::assert(true);
	spi::trans();
	spi::trans(index | _BV(6));
	spi::trans32(arg);
	spi::trans((crc << 1) | 1);
}

inline uint8_t sdhw_t::cmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	send(index, arg, crc);
	return response();
}

inline uint8_t sdhw_t::acmd(const uint8_t index, const uint32_t arg, const uint8_t crc)
{
	uint8_t res = cmd(APP_CMD);
	if (res > 0x01)
		return res;
	return cmd(index, arg, crc);
}

inline bool sdhw_t::streamStart(const bool rw, const uint32_t addr)
{
	if (!dataInit(rw, Multiple, addr))
		return false;
	return true;
}

inline uint8_t sdhw_t::nextByte(void)
{
	if (counter % 512 == 0) {
		readCRC();
		readInit();
	}
	counter++;
	return spi::trans();
}

#endif
