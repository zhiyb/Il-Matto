#include <string.h>
#include "dirent.h"

dirent __dirent__[MAX_DIRENT_CNT];
DIR __dir__[MAX_DIRENT_CNT];
uint32_t __current_dir__ = 0;
uint8_t __allocated_dir__ = 0, errno = 0;

class fs_t *fs;

void setfs(class fs_t *filesys)
{
	fs = filesys;
	__current_dir__ = fs->rootAddr();
}

static int8_t alloc_dir(void)
{
	for (uint8_t i = 0; i < MAX_DIRENT_CNT; i++) {
		if (__allocated_dir__ & (1 << i))
			continue;
		__allocated_dir__ |= 1 << i;
		__dir__[i].des = i;
		return i;
	}
	errno = ENFILE;
	return -1;
}

DIR *opendir(const char *name)
{
	if (name == NULL || *name == '\0') {
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
	DIR *dir = &__dir__[d];
	if (*name == '/') {
		dir->orig = fs->rootAddr();
		name++;
	} else
		dir->orig = __current_dir__;
nextlevel:
	rewinddir(dir);
	char buff[256];
	uint8_t i;
	for (i = 0; i < 255 && *name != '\0' && *name != '/'; name++, i++)
		buff[i] = *name;
	if (*name == '/')
		name++;
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
	if (strcmp(buff, ent->d_name) != 0)
		goto readent;
	if (!(ent->d_type & IS_DIR)) {
		closedir(dir);
		errno = ENOTDIR;
		return NULL;
	}
	dir->orig = ent->d_addr;
	goto nextlevel;
}

struct dirent *readdir(DIR *dir)
{
	struct dirent *ent = &__dirent__[dir->des];
	if (!fs->readDirEntry(dir, ent))
		return NULL;
	return ent;
}

void rewinddir(DIR *dir)
{
	dir->addr = dir->orig;
	dir->offset = 0;
}

int8_t closedir(DIR *dir)
{
	errno = EBADF;
	if (dir == NULL)
		return -1;
	if (dir->des > MAX_DIRENT_CNT)
		return -1;
	if ((__allocated_dir__ & (1 << dir->des)) == 0)
		return -1;
	errno = 0;
	__allocated_dir__ &= ~(1 << dir->des);
	return 0;
}
