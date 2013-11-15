#ifndef DISK_H
#define DISK_H

#include "sd.h"

class partition
{
public:
	partition(void) {}
	partition(uint8_t dat[16]);
	uint8_t type(void) const {return _type;}
	uint32_t begin(void) const {return _begin;}

private:
	uint8_t _type;
	uint32_t _begin;
};

class disk
{
public:
	enum error {SUCCEED = 0, NOT_INIT = 1, MBR_SIG_FAILED = 2};

	disk(void) {init();}
	void init(void);
	uint8_t status(void) const {return _status;}
	class partition& part(uint8_t i) {return _part[i];}

private:
	uint8_t _status;
	class partition _part[4];
};

#endif
