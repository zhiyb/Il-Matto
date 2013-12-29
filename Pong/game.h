#ifndef GAME_H
#define GAME_H

#define GAME_BGC	BLACK

extern struct game_struct {
	uint8_t mode;
	uint16_t score[2];
} game;

void game_main(void);
void game_over(uint8_t i);
void game_display_score(uint8_t i);
void game_connFailed(void);

#endif
