#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <tft.h>
#include <colours.h>
#include <macros.h>
#include "common.h"
#include "2048.h"

#define ROW	GAME_2048_ROW
#define COLUMN	GAME_2048_COLUMN

#define POS(r, c)	(((r) << 4) | (c))
#define R(p)		((p) >> 4)
#define C(p)		((p) & 0x0f)
#define INVALID		0xff

using namespace tft;
using namespace colours::b16;

const uint16_t game2048::colours[13][2] = {
	// Background	Foreground
	{COLOUR_565_888(0xfff5f2),	COLOUR_565_888(0x776e65),},	// 1
	{COLOUR_565_888(0xeee4da),	COLOUR_565_888(0x776e65),},	// 2
	{COLOUR_565_888(0xede0c8),	COLOUR_565_888(0x776e65),},	// 4
	{COLOUR_565_888(0xf2b179),	COLOUR_565_888(0xf9f6f2),},	// 8
	{COLOUR_565_888(0xf59563),	COLOUR_565_888(0xf9f6f2),},	// 16
	{COLOUR_565_888(0xf67c5f),	COLOUR_565_888(0xf9f6f2),},	// 32
	{COLOUR_565_888(0xf65e3b),	COLOUR_565_888(0xf9f6f2),},	// 64
	{COLOUR_565_888(0xedcf72),	COLOUR_565_888(0xf9f6f2),},	// 128
	{COLOUR_565_888(0xedcc61),	COLOUR_565_888(0xf9f6f2),},	// 256
	{COLOUR_565_888(0xedc850),	COLOUR_565_888(0xf9f6f2),},	// 512
	{COLOUR_565_888(0xedc53f),	COLOUR_565_888(0xf9f6f2),},	// 1024
	{COLOUR_565_888(0xedc22e),	COLOUR_565_888(0xf9f6f2),},	// 2048
	{COLOUR_565_888(0x3c3a32),	COLOUR_565_888(0xf9f6f2),},	// super
};

game2048::game2048()
{
	pressed = false;
	moved = false;
	restart();
}

void game2048::refresh()
{
	uint8_t rsize = width / ROW, csize = width / COLUMN;
	for (uint8_t r = 0; r < ROW; r++)
		for (uint8_t c = 0; c < COLUMN; c++) {
			uint16_t x = csize * c;
			uint16_t y = height - width + rsize * r;
			drawCell(x, y, data[r][c]);
		}
}

uint8_t game2048::toIndex(uint16_t v)
{
	if (v == 0)
		return INVALID;
	uint8_t i = 0;
	for (uint16_t mask = 1; mask != (1U << 12); mask <<= 1) {
		if (v & mask)
			break;
		i++;
	}
	return i;
}

void game2048::setColour(uint16_t v)
{
	uint8_t idx = toIndex(v);
	if (idx == INVALID)	// 0
		background = COLOUR_565_888(COLOUR_888(205, 192, 180));
	else {
		const uint16_t *clr = colours[idx];
		background = *clr;
		foreground = *(clr + 1);
	}
}

void game2048::drawCell(uint16_t x, uint16_t y, uint16_t v)
{
	setColour(v);
	const uint8_t space = 4;
	uint8_t rsize = width / ROW, csize = width / COLUMN;
	rectangle(x + space, y + space, \
			csize - space * 2, rsize - space * 2, background);
	if (v == 0)
		return;
	char buffer[6];
	uint8_t len = snprintf(buffer, 6, "%u", v);
	tft::x = x + (width / COLUMN - len * font->width) / 2;
	tft::y = y + (width / ROW - font->height) / 2;
	putString(buffer);
}

void game2048::restart()
{
	for (uint8_t r = 0; r < ROW; r++)
		for (uint8_t c = 0; c < COLUMN; c++)
			data[r][c] = 0;
	generate();
}

bool game2048::generate()
{
	// List available cell locations
	uint8_t available[ROW * COLUMN], *ptr = available;
	uint8_t i = 0;
	for (uint8_t r = 0; r < ROW; r++)
		for (uint8_t c = 0; c < COLUMN; c++) {
			if (data[r][c] == 0) {
				*ptr++ = POS(r, c);
				i++;
			}
		}
	// No available cells
	if (i == 0)
		return false;
	// Generate a new number at a random location
	i = rand() % i;
	uint8_t pos = available[i];
	data[R(pos)][C(pos)] = (rand() % 100) < 90 ? 2 : 4;
	return true;
}

void game2048::move(int8_t dx, int8_t dy)
{
	if (dx == 0 && dy == 0)
		return;
	// Reset merge table
	for (uint8_t r = 0; r < ROW; r++)
		for (uint8_t c = 0; c < COLUMN; c++)
			merged[r][c] = false;
	// Move cells
	bool moved = false;
	for (uint8_t ri = 0; ri < ROW; ri++)
		for (uint8_t ci = 0; ci < COLUMN; ci++) {
			// Actual row & column
			uint8_t r = dy == 1 ? ROW - 1 - ri : ri;
			uint8_t c = dx == 1 ? COLUMN - 1 - ci : ci;
			if (data[r][c] == 0)
				continue;
			uint8_t source = POS(r, c);
			uint8_t target = find(source, dy, dx);
			if (target != source) {
				moved = true;
				moveCell(source, target);
			}
		}
	// Generate new
	if (moved)
		generate();
	refresh();
}

void game2048::moveCell(uint8_t source, uint8_t target)
{
	uint16_t *pt = &data[R(target)][C(target)];
	uint16_t *ps = &data[R(source)][C(source)];
	if (*pt == *ps) {
		*pt = *pt * 2;
		merged[R(target)][C(target)] = true;
	} else
		*pt = *ps;
	*ps = 0;
}

uint8_t game2048::find(uint8_t source, int8_t dr, int8_t dc)
{
	int8_t r = R(source), c = C(source);
	bool inrange = false;
	do {
		r += dr;
		c += dc;
	} while ((inrange = (r + 1 != 0 && c + 1 != 0 && \
			r != ROW && c != COLUMN)) && \
			data[r][c] == 0);
	// Mergeability checking
	if (inrange && data[r][c] == data[R(source)][C(source)] && \
			merged[r][c] != true)
		return POS(r, c);
	return POS(r - dr, c - dc);
}

void game2048::enter()
{
	background = Black;
	foreground = White;
	setOrient(Portrait);
	clean();
	setFont(10, 16);
	putString("Game 2048!");
	rectangle(0, height - width, width, width, COLOUR_565_888(0xbbada0));
	refresh();
}

void game2048::pool()
{
	tft::x = 0;
	tft::y = font->height;
	switch(touch.status()) {
	case rTouch::Pressed:
		if (!pressed)
			pos[0] = touch.position();
		pressed = true;
		break;
	case rTouch::Moved:
		if (!moved)
			pos[1] = touch.position();
		moved = true;
		break;
	case rTouch::Idle:
		if (moved) {
			// Normalise
			int16_t dx = pos[1].x - pos[0].x;
			int16_t dy = pos[1].y - pos[0].y;
			if (abs(dx) > abs(dy)) {
				dx = dx >= 0 ? 1 : -1;
				dy = 0;
			} else {
				dx = 0;
				dy = dy >= 0 ? 1 : -1;
			}
			printf("%4d, %4d\n", dx, dy);
			move(dx, dy);
		}
		pressed = false;
		moved = false;
		break;
	}
}
