#ifndef FAT32_H
#define FAT32_H

#include "fs.h"
#include "hw.h"
#include "conv.h"

class fat32_t : public fs_t
{
public:
	enum ErrorTypes {Succeed = 0, DataStartFailed = 1, BytesPerSec_Failed = 2, NumFATs_Failed = 3, Signature_Failed = 4, DataStopFailed = 5};

	inline fat32_t(hw_t *hardware, const uint32_t addr);
	inline uint8_t err(void) const {return errno;}
	inline uint8_t chainRead(uint32_t clus = 0, uint32_t offset = 0);
	inline bool chainReadClose(void);

	virtual inline uint32_t rootAddr(void) const {return rootClus;}
	virtual inline bool readDirEntry(DIR *dir, struct dirent *ent);

private:
	inline uint32_t findNextClus(uint32_t clus);

	hw_t *hw;
	uint8_t secPerClus, errno;
	uint32_t FATAddr, clusAddr, rootClus, curClus, counter;
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

inline uint8_t fat32_t::chainRead(uint32_t clus, uint32_t offset)
{
	if (clus) {
		if (counter != 0xFF)
			hw->streamStop(hw_t::Read);
		counter = 0;
		curClus = clus;
		hw->streamStart(hw_t::Read, clusAddr + secPerClus * (clus - 2) + offset / 512);
		hw->skipBytes(offset % 512);
		return 0;
	}
	if (++counter % (secPerClus * 512) == 0) {
		hw->streamStop(hw_t::Read);
		curClus = findNextClus(curClus);
		hw->streamStart(hw_t::Read, clusAddr + secPerClus * (curClus - 2));
		counter = 0;
	}
	return hw->nextByte();
}

inline bool fat32_t::chainReadClose(void)
{
	counter = 0xFF;
	return hw->streamStop(hw_t::Read);
}

inline bool fat32_t::readDirEntry(DIR *dir, struct dirent *ent)
{
	uint8_t buff[12], index = 0;
	chainRead(dir->addr, dir->offset);
newEntry:
	dir->offset += 32;
	for (uint8_t i = 0; i < 12; i++)
		buff[i] = chainRead();
	if (buff[0] == 0xE5) {				// Deleted items
		for (uint8_t i = 0; i < 20; i++)
			chainRead();
		goto newEntry;
	}
	if (buff[0] == 0x00) {				// End of directory chain
		chainReadClose();
		return false;
	}
	if (buff[11] == 0x0F) {				// DIR_Attr field == Long file name
		uint8_t seq = buff[0] &= ~0x40;
		ent->d_name[seq * 13] = '\0';
		while (seq--) {
			dir->offset += 32;
			index = 0;
			for (uint8_t i = 0; i < 5; i++)
				ent->d_name[seq * 13 + index++] = conv::ucs2decode(buff[i * 2 + 1], buff[i * 2 + 2]);
			chainRead();			// Type, always 0x00
			chainRead();			// Checksum of DOS file name
			for (uint8_t i = 0; i < 6; i++) {
				char a = chainRead(), b = chainRead();
				ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			}
			chainRead();			// First cluster
			chainRead();			// Always 0x0000
			char a = chainRead(), b = chainRead();
			ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			a = chainRead(), b = chainRead();
			ent->d_name[seq * 13 + index++] = conv::ucs2decode(a, b);
			for (uint8_t i = 0; i < 12; i++)
				buff[i] = chainRead();	// For next loop
		}
		chainRead();				// Type byte
	} else						// 8.3 format short file name
		if (buff[11] & IS_DIR) {
			uint8_t type = chainRead();	// Type byte
			for (uint8_t i = 11; i > 0; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			for (uint8_t i = 0; i < 11; i++)
				ent->d_name[index++] = (type & (1 << 3)) ? tolower(buff[i]) : buff[i];
		} else {
			uint8_t type = chainRead();	// Type byte
			for (uint8_t i = 8; i > 0; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			for (uint8_t i = 0; i < 8 && buff[i] != '\0'; i++)
				ent->d_name[index++] = (type & (1 << 3)) ? tolower(buff[i]) : buff[i];
			uint8_t i;
			for (i = 11; i > 8; i--)
				if (buff[i - 1] != ' ')
					break;
				else
					buff[i - 1] = '\0';
			if (i != 8)
				ent->d_name[index++] = '.';
			for (i = 0; i < 3; i++)
				ent->d_name[index++] = (type & (1 << 4)) ? tolower(buff[i + 8]) : buff[i + 8];
			ent->d_name[index++] = '\0';
		}
	ent->d_type = buff[11];
	for (uint8_t i = 0; i < 7; i++)
		chainRead();
	ent->d_addr = (uint32_t)chainRead() << 16;
	ent->d_addr |= (uint32_t)chainRead() << 24;
	for (uint8_t i = 0; i < 4; i++)
		chainRead();
	ent->d_addr |= (uint32_t)chainRead();
	ent->d_addr |= (uint32_t)chainRead() << 8;
	ent->d_size = (uint32_t)chainRead();
	ent->d_size |= (uint32_t)chainRead() << 8;
	ent->d_size |= (uint32_t)chainRead() << 16;
	ent->d_size |= (uint32_t)chainRead() << 24;
	chainReadClose();
	if (curClus != dir->addr) {
		dir->addr = curClus;
		dir->offset %= secPerClus * 512;
	}
	return true;
}

#endif
