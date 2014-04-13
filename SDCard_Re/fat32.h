#ifndef FAT32_H
#define FAT32_H

#include "fs.h"
#include "hw.h"

class fat32_t : public fs_t
{
	inline fat32_t(hw_t *hw, const uint32_t addr);
};

inline fat32_t::fat32_t(hw_t *hw, const uint32_t addr)
{
}

#endif
