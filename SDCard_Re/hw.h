#ifndef HW_H
#define HW_H

#include <inttypes.h>

class hw_t
{
public:
	enum RW {Read = 0, Write = 1};

	// Note: Only read 1 block (512Bytes) at any time
	virtual inline bool dataAddress(const bool rw, const uint32_t addr) {return false;}
	virtual inline uint8_t readNextByte(void) {return 0;}
};

#endif
