#ifndef HW_H
#define HW_H

#include <inttypes.h>

class hw_t
{
public:
	enum RW {Read = 0, Write = 1};

	virtual inline bool dataAddress(const bool rw, const uint32_t addr) {return false;}
	virtual inline bool dataStop(const bool rw) {return false;}
	virtual inline uint8_t readNextByte(void) {return 0;}
};

#endif
