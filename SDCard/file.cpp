#include <stdlib.h>
#include "file.h"
#include "dirent.h"

namespace op
{
	int fgetchar(FILE *fp);
	static int8_t alloc_file(void);
	static int fclose(uint8_t fd);

	file_t *__file__[MAX_FILE_CNT];
	uint8_t __allocated_file__ = 0;
	extern class fs_t *fs;
}

int op::fgetchar(FILE *fp)
{
	fs->activate(fp);
	file_t *file = fstruct(fp);
	if (file == NULL)
		return -1;
	if (file->offset == file->size)
		return -1;
	file->offset++;
	return fs->readChar();
}

static int8_t op::alloc_file(void)
{
	for (uint8_t i = 0; i < MAX_FILE_CNT; i++) {
		if (__allocated_file__ & (1 << i))
			continue;
		if ((__file__[i] = (file_t *)malloc(sizeof(file_t))) == NULL) {
			errno = ENOMEM;
			return -1;
		}
		__allocated_file__ |= 1 << i;
		__file__[i]->des = i;
		return i;
	}
	errno = ENFILE;
	return -1;
}

static int op::fclose(uint8_t fd)
{
	errno = EBADF;
	if (fd >= MAX_DIRENT_CNT)
		return -1;
	if ((__allocated_file__ & (1 << fd)) == 0)
		return -1;
	errno = 0;
	free(__file__[fd]);
	__allocated_file__ &= ~(1 << fd);
	return 0;
}

FILE *op::fopen(const char *path, const char *mode)
{
	uint8_t fd;
	if ((int8_t)(fd = alloc_file()) == -1)
		return NULL;
	file_t *file = __file__[fd];
	if (!fs->readFileInfo(path, file)) {
		uint8_t err = errno;
		if (fclose(fd) == 0)
			errno = err;
		return NULL;
	}
	if ((file->fp = fdevopen(NULL, op::fgetchar)) == NULL) {
		fclose(fd);
		errno = ENOMEM;
		return NULL;
	}
	return file->fp;
}

struct file_t *op::fstruct(FILE *fp)
{
	for (uint8_t i = 0; i < MAX_FILE_CNT; i++) {
		if (!(__allocated_file__ & (1 << i)))
			continue;
		errno = 0;
		if (__file__[i]->fp == fp)
			return __file__[i];
	}
	errno = EBADF;
	return NULL;
}

int op::fclose(FILE *fp)
{
	fs->readClose();
	return fclose(fstruct(fp)->des);
}
