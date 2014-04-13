#ifndef DIRENT_H
#define DIRENT_H

typedef DIR uint8_t;	// Simply a descriptor

struct dirent
{
	char d_name[256];
};

#endif
