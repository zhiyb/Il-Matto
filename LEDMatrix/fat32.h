#ifndef FAT32_H
#define FAT32_H

#include "fs.h"
#include "hw.h"
#include "conv.h"
#include "file.h"

class fat32_t : public fs_t
{
public:
	enum ErrorTypes {Succeed = 0, DataStartFailed = 1, BytesPerSec_Failed = 2, NumFATs_Failed = 3, Signature_Failed = 4, DataStopFailed = 5};

	fat32_t(hw_t *hardware, const uint32_t addr);
	inline uint8_t err(void) const {return errno;}
	inline uint8_t chainRead(uint32_t clus = 0, uint32_t offset = 0);
	inline bool chainReadClose(void);

	virtual inline uint32_t rootAddr(void) const {return rootClus;}
	virtual bool readDirEntry(DIR *dir, struct dirent *ent);
	virtual bool readFileInfo(const char *path, file_t *file);
	virtual bool readFileInfo(const struct dirent *ent, file_t *file);
	virtual inline int readChar(void) {return chainRead();}
	virtual inline void readClose(void) {chainReadClose();}
	virtual inline void activate(FILE *fp);
	virtual inline void deactivate(void);

private:
	inline uint32_t findNextClus(uint32_t clus);

	hw_t *hw;
	uint8_t secPerClus, errno;
	uint32_t FATAddr, clusAddr, rootClus, curClus, counter;
};

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
	if (counter != 0 && counter++ % (secPerClus * 512) == 0) {
		hw->streamStop(hw_t::Read);
		curClus = findNextClus(curClus);
		hw->streamStart(hw_t::Read, clusAddr + secPerClus * (curClus - 2));
		counter = 1;
	}
	return hw->nextByte();
}

inline bool fat32_t::chainReadClose(void)
{
	if (counter == 0xFF)
		return true;
	counter = 0xFF;
	return hw->streamStop(hw_t::Read);
}

inline void fat32_t::activate(FILE *fp)
{
	if (hw->activated(this) && activated(fp))
		return;
	file_t *file = op::fstruct(fp);
	if (file == NULL)
		return;
	fs_t::activate(fp);
	if (!hw->activated(this))
		hw->activate(this);
	chainRead(file->addr, file->offset);
}

inline void fat32_t::deactivate(void)
{
	fs_t::deactivate();
	chainReadClose();
}

#endif
