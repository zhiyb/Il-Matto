#ifndef FAT32_H
#define FAT32_H

#include <stdio.h>
#include "disk.h"
#include "dir.h"
#include "conv.h"

class fat32_file
{
public:
	fat32_file(void) {index = 255;}
	bool open(class fat32 *f, uint8_t i, char *path);
	bool sopen(class fat32 *f, uint8_t i, char *path);
	inline bool opened(void) {return index != 255;}
	inline void close(void) {index = 255;}
	int getc(void);
	inline unsigned char sgetc(void);
	inline int getc(FILE *stream) {return getc();}
	inline uint32_t eof(void) {return remain;}
	inline void stop(void) {sd.stop();}

	class fat32 *fs;
	uint8_t secPerClus;
	uint32_t Cluster;
	
	uint8_t index;
	uint8_t buff[512];

	uint16_t offset;
	uint8_t sector;
	uint32_t cluster;
	uint32_t remain;

	uint32_t position;
	uint32_t size;
};

class fat32
{
public:
	enum Status {OK = 0, NOT_INIT, TYPE_ERROR, SIG_ERROR, FORMAT_ERROR};

	void test(void);
	inline fat32(void) {status = NOT_INIT;}
	inline fat32(const class partition& p) {init(p);}
	void init(const class partition& p);
	DIR *opendir(char *path);
	inline void closedir(DIR *dir) {dir->close();}
	struct dirent *readdir(DIR *dir);
	class fat32_file *fopen(char *path);
	class fat32_file *sfopen(char *path);
	struct dirent *_fopen(char *path);
	inline void _fclose(struct dirent *d) {((DIR *)d)->close();}
	inline void fclose(class fat32_file *f) {f->close();}
	inline uint32_t fatLookup(uint32_t clus);

	uint8_t status, errno;
	uint8_t secPerClus;
	uint32_t cluster;
	uint32_t fat;
	uint32_t root;
	uint32_t fatsize;
};

FILE *fat32_filedev(class fat32_file *f);

// Inline functions

#define CLUS2OFF(c) (Cluster + (c - 2) * secPerClus)

inline unsigned char fat32_file::sgetc(void)
{
	if (offset == 512) {				// Sector end
		offset = 0;
		if (++sector == secPerClus) {		// Cluster end
			sector = 0;
			cluster = fs->fatLookup(cluster);
		}
		sd.readBlockStart(CLUS2OFF(cluster) + sector);
	}
	offset++;
	remain--;
	return spi::trans();
}

#undef CLUS2OFF

inline uint32_t fat32::fatLookup(uint32_t clus)
{
	uint32_t off = clus * 4 / 512;
	sd.readBlockStart(fat + off);
	off = clus * 4 - off * 512;
	for (uint16_t i = 0; i < off; i++)
		spi::trans();
	uint32_t offset = spi::trans32();
	for (off += 4; off < 512; off++)
		spi::trans();
	return offset;
}

#endif
