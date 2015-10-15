#ifndef _2048_H
#define _2048_H

#include <stdint.h>
#include <rtouch.h>

#define GAME_2048_COLUMN	4
#define GAME_2048_ROW		4

class game2048
{
public:
	game2048();
	void enter();
	void pool();

private:
	void refresh();
	uint8_t toIndex(uint16_t v);
	void setColour(uint16_t v);
	void drawCell(uint16_t x, uint16_t y, uint16_t v);
	void restart();
	bool generate();
	void move(int8_t dx, int8_t dy);
	uint8_t find(uint8_t source, int8_t dr, int8_t dc);
	void moveCell(uint8_t source, uint8_t target);

	bool pressed, moved;
	rTouch::coord_t pos[2];
	uint16_t data[GAME_2048_ROW][GAME_2048_COLUMN];
	bool merged[GAME_2048_ROW][GAME_2048_COLUMN];
	static const uint16_t colours[13][2];
};

#endif
