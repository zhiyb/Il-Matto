#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include <eemem.h>
#include "tetris.h"
#include "tft.h"
#include "display.h"
#include "colour.h"
#include "timer.h"
#include "uart.h"

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
struct tetris object;

static uint16_t EEMEM NVhighScore;
static uint16_t EEMEM NVsaveValid;
static uint16_t EEMEM NVboard[Tetris_W][Tetris_H];
static struct tetris EEMEM NVobject;
static uint16_t EEMEM NVnext[4][4];
static uint16_t EEMEM NVtimerCnt;
static uint16_t EEMEM NVtimerMax;
static uint8_t EEMEM NVquick;
static uint8_t EEMEM NVoverflow;

const uint8_t PROGMEM type[7] = {
//	I	J	L	O	S	T	Z
	0xF0,	0x47,	0x17,	0x66,	0x36,	0x27,	0x63,
};
const uint16_t PROGMEM colour[7] = {
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
	return eeprom_read_word(&NVsaveValid);
}

uint16_t Tetris_getsavescore(void)
{
	return eeprom_read_word(&NVobject.score);
}

void save(void)
{
	// Save valid sign
	eeprom_update_word(&NVsaveValid, 1);
	// Save board
	eeprom_update_block(board, NVboard, sizeof(NVboard));
	// Save object
	eeprom_update_block(&object, &NVobject, sizeof(NVobject));
	// Save next unit
	eeprom_update_block(&next, &NVnext, sizeof(NVnext));
	// Timer
	eeprom_update_word(&NVtimerCnt, counter);
	eeprom_update_word(&NVtimerMax, maximum);
	eeprom_update_byte(&NVquick, quick);
	eeprom_update_byte(&NVoverflow, overflow);
}

void load(void)
{
	uint16_t highbak = object.high;

	// Load board
	eeprom_read_block(board, NVboard, sizeof(NVboard));
	// Load object
	eeprom_read_block(&object, &NVobject, sizeof(NVobject));
	object.high = highbak;
	// Load next unit
	eeprom_read_block(next, NVnext, sizeof(NVnext));
	// Timer
	counter = eeprom_read_word(&NVtimerCnt);
	maximum = eeprom_read_word(&NVtimerMax);
	quick = eeprom_read_byte(&NVquick);
	overflow = eeprom_read_byte(&NVoverflow);
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
	if (Tetris_getsavevalid() == 0) {
		Print_failed(1);
		getchar();
	}
	load();
	return 1;
}

void Tetris_mem_init(void)
{
	if (eeprom_first()) {
		eeprom_update_word(&NVhighScore, 0);
		eeprom_update_word(&NVsaveValid, 0);
	}
}

void Tetris_saveload(void)
{
	uint8_t index = 0;
	Print_saveload(INDEX_INIT);
print:
	Print_saveload(index);
get:
	switch (Uart_readch()) {
	case 's':	// Increase
		index += index != Tetris_S - 1;
		goto print;
	case 'w':	// Decrease
		index -= index != 0;
		goto print;
	case ' ':	// Return to game
	case 'a':
		refresh();
		return;
	case 'd':	// Apply
	case '\r':
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
	eeprom_update_word(&NVhighScore, object.high);
get:
	switch (Uart_readch()) {
	case 's':			// Enter Save & Load
	case 'w':
	case 'a':
	case 'd':
		Tetris_saveload();
		goto get;
	case 'q':
	case ' ':			// Return to game
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
	eeprom_update_word(&NVhighScore, object.high);
	getchar();
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
	object.high = eeprom_read_word(&NVhighScore);
	object.seed = rand();
	Tetris_generate();
	Tetris_generate();
	refresh();
	Timer_reset();
	Tetris_pause();
}
