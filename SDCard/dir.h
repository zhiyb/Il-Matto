#ifndef DIR_H
#define DIR_H

#define MAX_DIR_OPEN 4

#include <inttypes.h>

enum ErrNo {NOERR, EACCES, EMFILE, ENFILE, ENOTDIR, ENOENT, ENOMEM, EBADF};

typedef struct {
	uint32_t cluster;
	uint16_t offset;
} DIR;

struct dirent {
	uint32_t d_off;
	uint8_t d_type;
	char d_name[11];
};

extern DIR _dir_[MAX_DIR_OPEN];
extern struct dirent _dirent_[MAX_DIR_OPEN];
extern uint8_t _dir_use_;

inline uint8_t DIRindex(DIR *dir)
{
	for (uint8_t i = 0; i < MAX_DIR_OPEN; i++)
		if (dir == &_dir_[i])
			return i;
	return 0xFF;
}

#endif
