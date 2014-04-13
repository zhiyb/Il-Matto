#ifndef HW_H
#define HW_H

#include <inttypes.h>

class hw_t
{
public:
	enum RW {Read = 0, Write = 1};

	virtual inline bool streamStart(const bool rw, const uint32_t addr) {return false;}
	virtual inline bool streamStop(const bool rw) {return false;}
	virtual inline void skipBytes(uint16_t cnt);
	virtual inline uint8_t nextByte(void) {return 0;}
	virtual inline uint32_t nextBytes(const uint8_t cnt);
};

inline void hw_t::skipBytes(uint16_t cnt)
{
	while (cnt--)
		nextByte();
}

inline uint32_t hw_t::nextBytes(const uint8_t cnt)
{
	uint32_t res = 0;
	for (uint8_t i = 0; i < cnt; i++)
		res |= ((uint32_t)nextByte()) << (8 * i);
	return res;
}

#endif
