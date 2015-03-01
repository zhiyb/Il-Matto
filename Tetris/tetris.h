#ifndef _TETRIX_H
#define _TETRIX_H

#define Tetris_W 10
#define Tetris_H 20
#define Tetris_S 2	// Save index

#include <avr/io.h>

struct tetris {
	uint16_t space[4][4];
	int8_t x, y;
	uint8_t restart;
	uint16_t score, high;
	uint32_t seed;
};

extern uint16_t board[Tetris_W][Tetris_H], next[4][4];
extern struct tetris object;

void Tetris_init(void);
uint8_t Tetris_generate(void);
void Tetris_print(void);
uint8_t Tetris_rotate(uint8_t anti);
uint8_t Tetris_drop(void);
uint8_t Tetris_move(int8_t d);
void Tetris_place(void);
void Tetris_gameover(void);
void Tetris_pause(void);
void Tetris_saveload(void);
void Tetris_mem_init(void);
uint16_t Tetris_getsavescore(void);
uint16_t Tetris_getsavevalid(void);

#endif
