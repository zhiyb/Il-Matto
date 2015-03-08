#ifndef DIRENT_H
#define DIRENT_H

#include <inttypes.h>
#include "errno.h"
#include "dirstream.h"

// Note: less than 8
#define MAX_DIRENT_CNT	2

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

#include "fs.h"

namespace op
{
	DIR *opendir(const char *path);
	struct dirent *readdir(DIR *dir);
	void rewinddir(DIR *dir);
	int closedir(DIR *dir);
	void setfs(fs_t *filesys);
}

#endif
