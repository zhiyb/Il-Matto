#ifndef ANIMATION_H
#define ANIMATION_H

#include <inttypes.h>

namespace apps
{
	uint8_t image(const char *path);
	uint8_t image(struct dirent *ent);
	uint8_t animation(const char *path, const uint_t clr);
	uint8_t wav(const char *path);
}

#endif
