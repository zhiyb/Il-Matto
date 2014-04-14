#ifndef DIRENT_H
#define DIRENT_H

#include <inttypes.h>

// Note: less than 8
#define MAX_DIRENT_CNT	2

#define EACCESS	1
#define EMFILE	2
#define ENFILE	3
#define ENOTDIR	4
#define ENOENT	5
#define ENOMEM	6
#define ENOFS	7
#define EBADF	0xFF

#ifndef NULL
#define NULL	0
#endif

#define IS_DIR	(1 << 4)

struct dirent
{
	// Entry name
	char d_name[256];
	// Entry type & attribuate
	uint8_t d_type;
	// Entry size
	uint32_t d_size;
	// Entry base address
	uint32_t d_addr;
};

struct dirstream
{
	// Descriptor
	uint8_t des;
	// Original base address
	uint32_t orig;
	// Current base address
	uint32_t addr;
	// Current offset
	uint32_t offset;
};

typedef struct dirstream DIR;

extern uint8_t errno;

#include "fs.h"

void setfs(class fs_t *filesys);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
void rewinddir(DIR *dir);
int8_t closedir(DIR *dir);

#endif
