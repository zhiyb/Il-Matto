#ifndef FS_H
#define FS_H

class fs_t;

#include <stdio.h>
#include "file.h"
#include "dirstream.h"

class fs_t
{
public:
	fs_t(void) {deactivate();}
	virtual inline uint32_t rootAddr(void) const {return 0;}
	virtual inline bool readDirEntry(DIR *dir, struct dirent *ent) {return false;}
	virtual inline bool readFileInfo(const char *path, file_t *file) {return false;}
	virtual inline bool readFileInfo(const struct dirent *ent, file_t *file) {return false;}
	virtual inline int readChar(void) {return -1;}
	virtual inline void readClose(void) {}
	virtual inline void activate(FILE *fp) {act = fp;}
	virtual inline bool activated(FILE *fp) {return act == fp;}
	virtual inline void deactivate(void) {act = 0;}

protected:
	FILE *act;
};

#include "dirent.h"

#endif
