#ifndef DIRSTREAM_H
#define DIRSTREAM_H

struct dirstream
{
	// Descriptor
	uint8_t des;
	// Original base address
	uint32_t orig;
	// Current base address
	uint32_t addr;
	// Current offset
	uint32_t offset;
};

typedef struct dirstream DIR;

#endif
