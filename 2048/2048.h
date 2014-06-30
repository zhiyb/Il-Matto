#ifndef _2048_H
#define _2048_H

#include <inttypes.h>

#define COLUMN	4
#define ROW	4

class game2048
{
public:
	game2048(void);
	uint8_t field(const uint8_t r, const uint8_t c) const {return data[r][c];}

private:
	uint8_t data[ROW][COLUMN];
};

#endif
