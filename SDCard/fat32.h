#ifndef FAT32_H
#define FAT32_H

#include "disk.h"

class fat32
{
public:
	enum Status {OK, NOT_INIT, TYPE_ERROR};

	fat32(void) {_status = NOT_INIT;}
	fat32(const class partition& p) {init(p);}
	void init(const class partition& p);
	uint8_t status(void) const {return _status;}

private:
	uint8_t _status;
};

#endif
