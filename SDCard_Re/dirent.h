#ifndef DIRENT_H
#define DIRENT_H

#include <inttypes.h>

#define MAX_DIRENT_CNT	2

struct dirent
{
	// Entry name
	char d_name[256];
	// Entry attribuate
	uint8_t d_attr;
	// Entry cluster
	uint32_t d_clus;
	// Entry size
	uint32_t d_size;
	// Original cluster
	uint32_t d_orgClus;
	// Current offset
	uint32_t d_offset;
	// Current cluster
	uint32_t d_curClus;
};

typedef struct __dirstream DIR;

extern dirent __dirent__[MAX_DIRENT_CNT];

#endif
