#ifndef FAT32_H
#define FAT32_H

#include "fs.h"
#include "hw.h"

class fat32_t : public fs_t
{
public:
	enum ErrorTypes {Succeed = 0, DataStartFailed = 1, BytesPerSec_Failed = 2, NumFATs_Failed = 3, Signature_Failed = 4, DataStopFailed = 5};

	inline fat32_t(hw_t *hardware, const uint32_t addr);
	inline uint8_t err(void) const {return errno;}
	inline uint8_t chainRead(uint32_t clus = 0);
	inline bool chainReadClose(void) {return hw->streamStop(hw_t::Read);}

//private:
	inline uint32_t findNextClus(uint32_t clus);

	hw_t *hw;
	uint8_t secPerClus, counter, errno;
	uint32_t FATAddr, clusAddr, rootClus, curClus;
};

inline fat32_t::fat32_t(hw_t *hardware, const uint32_t addr)
{
	hw = hardware;
	if (!hw->streamStart(hw_t::Read, addr)) {
		errno = 1;
		return;
	}
	hw->skipBytes(0x0B);
	if (hw->nextBytes(2) != 512) {			// Bytes per sector
		errno = 2;
		goto ret;
	}
	secPerClus = hw->nextByte();			// Sectors per cluster
	FATAddr = addr + hw->nextBytes(2);		// Number of reserved sectors
	if (hw->nextByte() != 2) {			// Number of FATs
		errno = 3;
		goto ret;
	}
	hw->skipBytes(0x24 - 0x10 - 1);
	clusAddr = FATAddr + 2 * hw->nextBytes(4);	// Sectors per FAT
	hw->skipBytes(0x2C - (0x24 + 3) - 1);
	rootClus = hw->nextBytes(4);			// Root directory first cluster
	hw->skipBytes(0x1FE - (0x2C + 3) - 1);
	if (hw->nextBytes(2) != 0xAA55) {		// Signature
		errno = 4;
		goto ret;
	}
	errno = 0;
	counter = 0xFF;
ret:
	if (!hw->streamStop(hw_t::Read))
		errno = 5;
}

inline uint32_t fat32_t::findNextClus(uint32_t clus)
{
	hw->streamStart(hw_t::Read, FATAddr + clus * 4 / 512);
	hw->skipBytes((clus * 4) % 512);
	clus = hw->nextBytes(4);
	hw->streamStop(hw_t::Read);
	return clus;
}

inline uint8_t fat32_t::chainRead(uint32_t clus)
{
	if (clus) {
		if (counter != 0xFF)
			hw->streamStop(hw_t::Read);
		counter = 0;
		curClus = clus;
		hw->streamStart(hw_t::Read, clusAddr + secPerClus * (clus - 2));
	}
	if (++counter % secPerClus == 0) {
		hw->streamStop(hw_t::Read);
		curClus = findNextClus(curClus);
		hw->streamStart(hw_t::Read, clusAddr + secPerClus * (curClus - 2));
		counter = 0;
	}
	return hw->nextByte();
}

#endif
