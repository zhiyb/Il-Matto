#ifndef MBR_H
#define MBR_H

#include "hw.h"

class mbr_t
{
public:
	enum FSTypes {Empty = 0x00, FAT32_OLD = 0x0B, FAT32 = 0x0C};
	enum ErrorTypes {Succeed = 0, DataStartFailed = 1, NoMBRExist = 2, DataStopFailed = 3};

	inline mbr_t(hw_t *hw);
	inline uint8_t err(void) const {return errno;}
	inline uint8_t type(const uint8_t index) const {return _type[index];}
	inline uint32_t address(const uint8_t index) const {return _addr[index];}

private:
	inline void setEntry(const uint8_t index, uint8_t data[]);

	uint8_t _type[4], errno;
	uint32_t _addr[4];
};

inline mbr_t::mbr_t(hw_t *hw)
{
	if (!hw->streamStart(hw_t::Read, 0)) {
		errno = 1;
		return;
	}
	hw->skipBytes(446);
	for (uint8_t j = 0; j < 4; j++) {
		uint8_t data[16];
		for (uint8_t i = 0; i < 16; i++)
			data[i] = hw->nextByte();
		setEntry(j, data);
	}
	if (hw->nextBytes(2) != 0xAA55)
		errno = 2;
	else
		errno = 0;
	if (!hw->streamStop(hw_t::Read)) {
		errno = 3;
		return;
	}
}

inline void mbr_t::setEntry(const uint8_t index, uint8_t data[])
{
	_type[index] = data[4];
	for (uint8_t i = 0; i < 4; i++) {
		_addr[index] <<= 8;
		_addr[index] |= data[8 + (3 - i)];
	}
}

#endif
