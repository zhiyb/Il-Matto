#ifndef DISK_H
#define DISK_H

#include "sd.h"

class partition
{
public:
	inline partition(void) {}
	inline partition(uint8_t dat[16]);
	inline uint8_t type(void) const {return _type;}
	inline uint32_t begin(void) const {return _begin;}

private:
	uint8_t _type;
	uint32_t _begin;
};

class disk
{
public:
	enum error {SUCCEED = 0, NOT_INIT, SIG_ERROR};

	inline disk(void) {init();}
	inline void init(void);
	inline uint8_t status(void) const {return _status;}
	inline class partition& part(uint8_t i) {return _part[i];}

private:
	uint8_t _status;
	class partition _part[4];
};

// Defined as inline to execute faster

inline partition::partition(uint8_t dat[16])
{
	_type = dat[4];
	for (uint8_t i = 0; i < 4; i++) {
		_begin <<= 8;
		_begin |= dat[8 + (3 - i)];
	}
}

#include <stdio.h>
inline void disk::init(void)
{
	if (sd.version() == 0) {
		_status = NOT_INIT;
		return;
	}
	_status = SUCCEED;
	uint8_t block[512];
	sd.readBlock(0, block);
	if (block[510] != 0x55 || block[511] != 0xAA) {
		_status = SIG_ERROR;
		return;
	}
	for (uint8_t i = 0; i < 3; i++)
		_part[i] = partition(&block[446 + 16 * i]);
}

#endif
