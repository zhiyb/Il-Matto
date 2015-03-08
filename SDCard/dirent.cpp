#include <string.h>
#include <stdlib.h>
#include "dirent.h"

namespace op
{
	static int8_t alloc_dir(void);

	dirent *__dirent__[MAX_DIRENT_CNT];
	DIR *__dir__[MAX_DIRENT_CNT];
	uint32_t __current_dir__ = 0;
	uint8_t __allocated_dir__ = 0;
	class fs_t *fs;
}

void op::setfs(class fs_t *filesys)
{
	fs = filesys;
	__current_dir__ = fs->rootAddr();
}

static int8_t op::alloc_dir(void)
{
	for (uint8_t i = 0; i < MAX_DIRENT_CNT; i++) {
		if (__allocated_dir__ & (1 << i))
			continue;
		__allocated_dir__ |= 1 << i;
		if ((__dir__[i] = (DIR *)malloc(sizeof(DIR))) == NULL || (__dirent__[i] = (struct dirent *)malloc(sizeof(struct dirent))) == NULL) {
			errno = ENOMEM;
			return -1;
		}
		__dir__[i]->des = i;
		return i;
	}
	errno = ENFILE;
	return -1;
}

DIR *op::opendir(const char *path)
{
	if (path == NULL || *path == '\0') {
		errno = ENOENT;
		return NULL;
	}
	if (__current_dir__ == 0) {
		errno = ENOFS;
		return NULL;
	}
	uint8_t d;
	if ((int8_t)(d = alloc_dir()) == -1)
		return NULL;
	DIR *dir = __dir__[d];
	if (*path == '/') {
		dir->orig = fs->rootAddr();
		path++;
	} else
		dir->orig = __current_dir__;
nextlevel:
	rewinddir(dir);
	char buff[256];
	uint8_t i;
	for (i = 0; i < 255 && *path != '\0' && *path != '/'; path++, i++)
		buff[i] = *path;
	if (*path == '/')
		path++;
	if (i == 255) {
		closedir(dir);
		errno = ENOENT;
		return NULL;
	}
	if (i == 0) {
		errno = 0;
		return dir;
	}
	buff[i] = '\0';
readent:
	struct dirent *ent = readdir(dir);
	if (ent == NULL) {
		closedir(dir);
		errno = ENOENT;
		return NULL;
	}
	if (strcasecmp(buff, ent->d_name) != 0)
		goto readent;
	if (!(ent->d_type & IS_DIR)) {
		closedir(dir);
		errno = ENOTDIR;
		return NULL;
	}
	dir->orig = ent->d_addr;
	goto nextlevel;
}

struct dirent *op::readdir(DIR *dir)
{
	struct dirent *ent = __dirent__[dir->des];
	if (!fs->readDirEntry(dir, ent))
		return NULL;
	return ent;
}

void op::rewinddir(DIR *dir)
{
	dir->addr = dir->orig;
	dir->offset = 0;
}

int op::closedir(DIR *dir)
{
	errno = EBADF;
	if (dir == NULL)
		return -1;
	if (dir->des >= MAX_DIRENT_CNT)
		return -1;
	if ((__allocated_dir__ & (1 << dir->des)) == 0)
		return -1;
	errno = 0;
	free(__dir__[dir->des]);
	free(__dirent__[dir->des]);
	__allocated_dir__ &= ~(1 << dir->des);
	return 0;
}
