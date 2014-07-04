#ifndef FILE_H
#define FILE_H

#include <inttypes.h>
#include <stdio.h>
#include "errno.h"

// Note: less than 8
#define MAX_FILE_CNT	3

struct file_t
{
	// Descriptor
	uint8_t des;
	// FILE pointer
	FILE *fp;
	// Open mode
	uint8_t mode;
	// Original address
	uint32_t addr;
	// Current offset
	uint32_t offset;
	// File size
	uint32_t size;
};

#include "dirent.h"

namespace op
{
	FILE *fopen(const char *path, const char *mode);
	FILE *fopen(const struct dirent *ent, const char *mode);
	struct file_t *fstruct(FILE *fp);
	int fclose(FILE *fp);
}

#endif
