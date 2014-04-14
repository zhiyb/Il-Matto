#ifndef FILE_H
#define FILE_H

struct file_t
{
	// Descriptor
	uint8_t des;
	// FILE pointer
	FILE *fp;
};

extern uint8_t errno;

#endif
