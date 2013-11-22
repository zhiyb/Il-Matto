#ifndef FAT32_H
#define FAT32_H

#include "disk.h"
#include "dir.h"

class fat32
{
public:
	enum Status {OK = 0, NOT_INIT, TYPE_ERROR, SIG_ERROR, FORMAT_ERROR};

	void test(void);
	inline fat32(void) {_status = NOT_INIT;}
	inline fat32(const class partition& p) {init(p);}
	void init(const class partition& p);
	inline uint8_t status(void) const {return _status;}
	inline uint8_t errno(void) const {return _errno;}
	DIR *opendir(char *path);
	uint8_t closedir(DIR *dir);
	struct dirent *readdir(DIR *dir);
	FILE *fopen_read(char *path);

private:
	uint32_t fatLookup(uint32_t cluster);

	uint8_t _status, _errno;
	uint32_t _cluster;
	uint32_t _fat;
	uint32_t _root;
	uint32_t _fatsize;
	uint8_t _secPerClus;
};

#endif
