#include <stdlib.h>
#include "2048.h"

game2048::game2048(void)
{
	for (uint8_t r = 0; r < ROW; r++)
		for (uint8_t c = 0; c < COLUMN; c++)
			data[r][c] = 0;
	srand(0);
}
