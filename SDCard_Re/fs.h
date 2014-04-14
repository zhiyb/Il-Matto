#ifndef FS_H
#define FS_H

#include "dirent.h"

class fs_t
{
public:
	virtual inline uint32_t rootAddr(void) const {return 0;}
	virtual inline bool readDirEntry(DIR *dir, struct dirent *ent) {return false;}
};

#endif
