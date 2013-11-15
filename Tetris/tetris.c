#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "tetris.h"
#include "tft.h"
#include "display.h"
#include "colour.h"
#include "rand.h"
#include "timer.h"
#include "rotary.h"
#include "eemem.h"

#define NO_ERROR 0
#define OUT_OF_RANGE 1
#define UNIT_EXIST 2

#define UNIT_NEXT 0
#define UNIT_OBJECT 1

#define SWAP(x, y) { \
	(x) = (x) ^ (y); \
	(y) = (x) ^ (y); \
	(x) = (x) ^ (y); \
}

uint8_t err;
uint16_t board[Tetris_W][Tetris_H], next[4][4];
struct tetrix object;

const uint8_t type[7] PROGMEM = {
//	I	J	L	O	S	T	Z
	0xF0,	0x47,	0x17,	0x66,	0x36,	0x27,	0x63,
};
const uint16_t colour[7] PROGMEM = {
//	I	J	L	O	S	T	Z
	0x07FF,	0x001F,	0xFBE0,	0xFFE0,	0x07E0,	0x781F,	0xF800,
};

uint8_t valid(void);
uint8_t near(void);
void topcenter(uint16_t (*space)[4]);
void rotate(uint16_t (*space)[4], uint8_t anti);
uint8_t clean(uint8_t l);
void refresh(void);
uint8_t saveload(uint8_t index);
void save(void);
void load(void);

uint16_t Tetris_getsavevalid(void)
{
	return eeprom_read_word(EE_TETRIS_SAVE);
}

uint16_t Tetris_getsavescore(void)
{
	uint8_t *p = (uint8_t *)EE_TETRIS_SAVE;
	p += 2 + (Tetris_H * Tetris_W) * 2 + \
	     (uint8_t *)&object.score - (uint8_t *)&object;
	return eeprom_read_word((uint16_t *)p);
}

void save(void)
{
	uint8_t x, y, *o = (uint8_t *)&object;
	uint16_t *p = EE_TETRIS_SAVE;

	// Save valid sign
	eeprom_update_word(p++, 1);

	// Save board
	for (y = 0; y < Tetris_H; y++)
		for (x = 0; x < Tetris_W; x++)
			eeprom_update_word(p++, board[x][y]);

	// Save object
	for (x = 0; x < sizeof(object); x++) {
		eeprom_update_byte(((uint8_t *)p) + x % 2, *o++);
		p += x % 2;
	}
	if (sizeof(object) % 2)
		p++;

	// Save next unit
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			eeprom_update_word(p++, next[x][y]);

	// Timer
	eeprom_update_word(p++, counter);
	eeprom_update_word(p++, maximum);
	eeprom_update_byte((uint8_t *)p, quick);
	eeprom_update_byte(((uint8_t *)p) + 1, overflow);
}

void load(void)
{
	uint8_t x, y, *o = (uint8_t *)&object;
	uint16_t *p = EE_TETRIS_SAVE + 1, highbak = object.high;

	// Load board
	for (y = 0; y < Tetris_H; y++)
		for (x = 0; x < Tetris_W; x++)
			board[x][y] = eeprom_read_word(p++);

	// Load object
	for (x = 0; x < sizeof(object); x++) {
		*o++ = eeprom_read_byte(((uint8_t *)p) + x % 2);
		p += x % 2;
	}
	object.high = highbak;
	if (sizeof(object) % 2)
		p++;

	// Load next unit
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			next[x][y] = eeprom_read_word(p++);

	// Timer
	counter = eeprom_read_word(p++);
	maximum = eeprom_read_word(p++);
	quick = eeprom_read_byte((uint8_t *)p);
	overflow = eeprom_read_byte(((uint8_t *)p) + 1);
}

uint8_t saveload(uint8_t index)
{
	if (index)
		goto load;
	// Save
	Print_saving(0);
	save();
	return 1;
load:	// Load
	if (eeprom_read_word(EE_TETRIS_SAVE) == 0) {
		Print_failed(1);
		while (1)
			switch (ROE_get()) {
			case ROE_N:
				continue;
			case ROE_SW1:
				while (ROE_get() == ROE_SW1);
				return 0;
			case ROE_SW2:
				while (ROE_get() == ROE_SW2);
			default:
				return 0;
			}
	}
	load();
	return 1;
}

void Tetris_mem_init(void)
{
	if (EEPROM_first()) {
		eeprom_update_word(EE_TETRIS_HIGH, 0);
		eeprom_update_word(EE_TETRIS_SAVE, 0);
	}
}

void Tetris_saveload(void)
{
	uint8_t index = 0;
	Print_saveload(INDEX_INIT);
print:
	Print_saveload(index);
get:
	switch (ROE_get()) {
	case ROE_CW1:
	case ROE_CW2:	// Increase
		index += index != Tetris_S - 1;
		goto print;
	case ROE_CCW1:
	case ROE_CCW2:	// Decrease
		index -= index != 0;
		goto print;
	case ROE_SW1:	// Return to game
		while (ROE_get() == ROE_SW1);
		refresh();
		return;
	case ROE_SW2:	// Apply
		while (ROE_get() == ROE_SW2);
		if (saveload(index)) {
			refresh();
			return;
		}
		goto print;
	default:
		goto get;
	}
}

void refresh(void)
{
	Print_board();
	Print_object();
	Print_highest();
	Print_score();
	Print_next();
	Print_Speed();
}

void Tetris_pause(void)
{
	Timer_disable();
	Print_pause();
	TFT_backgroundLight(1);
	eeprom_update_word(EE_TETRIS_HIGH, object.high);
	while (ROE_get() == ROE_SW1);	// Wait for button 1 release
get:
	switch (ROE_get()) {
	case ROE_SW2:			// Enter Save & Load
		while (ROE_get() == ROE_SW2);
		Tetris_saveload();
		goto get;
	case ROE_SW1:			// Return to game
		while (ROE_get() == ROE_SW1);
		break;
	default:			// Else
		goto get;
	}
	Print_start();
	Timer_enable();
}

void Tetris_gameover(void)
{
	Timer_disable();
	Print_gameover();
	eeprom_update_word(EE_TETRIS_HIGH, object.high);
	while (ROE_get() != ROE_SW1);
	while (ROE_get() == ROE_SW1);
	Tetris_init();
}

uint8_t Tetris_move(int8_t d)
{
	object.x += d;
	if (valid())
		return 1;
	object.x -= d;
	return 0;
}

uint8_t valid(void)
{
	uint8_t x, y;
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++) {
			if (object.space[x][y] == Colour_Unit_Empty)
				continue;
			if (board[object.x + x][object.y + y] != \
					Colour_Unit_Empty) {
				err = UNIT_EXIST;
				return 0;
			}
			if (object.y + y >= Tetris_H) {
				err = OUT_OF_RANGE;
				return 0;
			}
			if (object.x + x < 0) {
				err = OUT_OF_RANGE;
				return 0;
			}
			if (object.x + x >= Tetris_W) {
				err = OUT_OF_RANGE;
				return 0;
			}
		}
	err = NO_ERROR;
	return 1;
}

uint8_t Tetris_drop(void)
{
	object.y++;
	if (valid())
		return 1;
	object.y--;
	return 0;
}

uint8_t clean(uint8_t l)
{
	uint8_t x, y;
	for (x = 0; x < Tetris_W; x++)
		if (board[x][l] == Colour_Unit_Empty)
			return 0;
	for (y = l; y > 0; y--)
		for (x = 0; x < Tetris_W; x++)
			board[x][y] = board[x][y - 1];
	for (x = 0; x < Tetris_W; x++)
		board[x][0] = Colour_Unit_Empty;
	return 1;
}

void Tetris_clean(void)
{
	uint8_t x, y, l = 0;
	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++)
			if (object.space[x][y] != Colour_Unit_Empty)
				break;
		if (x == 4)
			continue;
		if (clean(object.y + y)) {
			l++;
			Print_board();
		}
	}
	object.score += l * l * 10;
	if (l)
		Timer_faster();
	Print_Speed();
}

void Tetris_place(void)
{
	uint8_t x, y;
	Timer_disable();
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++) {
			if (object.space[x][y] == Colour_Unit_Empty)
				continue;
			board[object.x + x][object.y + y] = object.space[x][y];
		}
	object.score += 1;
	Tetris_clean();
	Print_score();
	if (object.score > object.high) {
		object.high = object.score;
		Print_highest();
	}
	Timer_enable();
}

uint8_t Tetris_rotate(uint8_t anti)
{
	uint8_t x, y;
	uint16_t tmp[4][4];
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			tmp[x][y] = object.space[x][y];
	rotate(object.space, anti);
	if (near())
		return 1;
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			object.space[x][y] = tmp[x][y];
	return 0;
}

uint8_t near(void)
{
	uint8_t x, y;
	if (valid())
		return 1;
	x = 0;
	y = (OUT_OF_RANGE << 4) | OUT_OF_RANGE;
retry:
	x++;
	if (y & (OUT_OF_RANGE << 4)) {
		if (Tetris_move(-x))
			return 1;
		y &= 0x0F;
		y |= err << 4;
	}
	if (y & OUT_OF_RANGE) {
		if (Tetris_move(x))
			return 1;
		y &= 0xF0;
		y |= err;
	}
	if (x < 3 && y & (OUT_OF_RANGE << 4 | OUT_OF_RANGE))
		goto retry;
	return 0;
}

void topcenter(uint16_t (*space)[4])
{
	uint8_t x, y;
top:	// To top
	for (x = 0; x < 4; x++)
		if (space[x][0] != Colour_Unit_Empty)
			goto center;
	for (y = 0; y < 3; y++)
		for (x = 0; x < 4; x++)
			space[x][y] = space[x][y + 1];
	for (x = 0; x < 4; x++)
		space[x][3] = Colour_Unit_Empty;
	goto top;
center:	// To center
	for (y = 0; y < 4; y++)
		if (space[0][y] != Colour_Unit_Empty)
			goto center_left;
	for (y = 0; y < 4; y++)
		if (space[1][y] != Colour_Unit_Empty)
			return;
	// Move 1 pixel left
	for (x = 0; x < 3; x++)
		for (y = 0; y < 4; y++)
			space[x][y] = space[x + 1][y];
	for (y = 0; y < 4; y++)
		space[3][y] = Colour_Unit_Empty;
	return;
center_left:
	for (y = 0; y < 4; y++)
		if (space[4 - 1][y] != Colour_Unit_Empty)
			return;
	// Move 1 pixel right
	for (x = 3; x > 0; x--)
		for (y = 0; y < 4; y++)
			space[x][y] = space[x - 1][y];
	for (y = 0; y < 4; y++)
		space[0][y] = Colour_Unit_Empty;
}

void rotate(uint16_t (*space)[4], uint8_t anti)
{
	uint8_t x, y;
	uint16_t tmp[4][4];
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			if (anti)
				tmp[x][y] = space[x][y];
			else
				tmp[x][y] = space[x][y];
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			if (anti)
				space[y][4 - 1 - x] = tmp[x][y];
			else
				space[4 - 1 - y][x] = tmp[x][y];
	topcenter(space);
}

uint8_t Tetris_generate(void)
{
	uint8_t r, x, y, t;
	uint16_t c;

	// Set current seed & random generate
	srand(object.seed);
	r = rand() % 7;
	t = pgm_read_byte(&(type[r]));
	c = pgm_read_word(&(colour[r]));

	// Copy from next unit
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			object.space[x][y] = next[x][y];
	object.x = (Tetris_W - 4) / 2;
	object.y = 0;
	if (!near())
		return 0;

	// Generate next unit
	for (y = 0; y < 2; y++)
		for (x = 0; x < 4; x++)
			next[x][y] = (t & (0x80 >> (y * 4 + x))) ? \
					     c : Colour_Unit_Empty;
	for (; y < 4; y++)
		for (x = 0; x < 4; x++)
			next[x][y] = Colour_Unit_Empty;
	switch (rand() % 4) {
	case 1:
		rotate(next, 0);
		break;
	case 2:
		if (r == 0)	// No need to flip 'I'
			break;
		for (x = 0; x < 4; x++)
			SWAP(next[x][0], next[4 - 1 - x][1]);
		topcenter(next);
		break;
	case 3:
		rotate(next, 1);
	}

	// Generate new seed
	object.seed = rand();
	return 1;
}

void Tetris_init(void)
{
	uint8_t x, y;
	for (y = 0; y < Tetris_H; y++)
		for (x = 0; x < Tetris_W; x++)
			board[x][y] = Colour_Unit_Empty;
	object.score = 0;
	object.restart = 1;
	object.high = eeprom_read_word(EE_TETRIS_HIGH);
	object.seed = rand();
	Tetris_generate();
	Tetris_generate();
	refresh();
	Timer_reset();
	Tetris_pause();
}
