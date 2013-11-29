#ifndef DIR_H
#define DIR_H

#define MAX_DIR_OPEN 2
#define MAX_FILE_OPEN 1

#include <inttypes.h>

enum ErrNo {NOERR, EACCES, EMFILE, ENFILE, ENOTDIR, ENOENT, ENOMEM, EBADF};

struct dirent
{
public:
	uint8_t d_type;
	uint32_t d_off, d_size;
	char d_name[11];
};

typedef struct _dir_struct_: public dirent {
public:
	inline _dir_struct_(void) {index = 255;}
	inline bool open(uint8_t i);
	inline bool opened(void) {return index != 255;}
	inline void close(void) {index = 255;}

	uint8_t index;
	uint16_t offset;
	uint32_t cluster;
} DIR;

extern DIR _dir_[MAX_DIR_OPEN];

// inline functions
bool _dir_struct_::open(uint8_t i)
{
	if (opened())
		return false;
	index = i;
	return true;
}

#endif
