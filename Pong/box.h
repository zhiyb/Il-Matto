#ifndef BOX_H
#define BOX_H

#include "libili9341/tft.h"
#include "game.h"
#include "timer.h"
#include "connect.h"
#include "sound.h"

#define BOX_SIZE 5
#define BOX_INITX ((tft.w - BOX_SIZE) / 2)
#define BOX_INITY ((tft.h - BOX_SIZE) / 2)
#define BOX_FGC YELLOW
#define BOX_BGC (0x0841 * 2) //LTY
//#define BOX_TRACE_CLR_CHANGE

struct {
	uint16_t x, y;
	uint8_t dx, dy, n, e;
} box;

#include "bar.h"

static inline void box_show(uint16_t colour)
{
	if (box.x > tft.w)
		tft_rectangle(box.x, box.y, tft.w - box.x, BOX_SIZE, colour);
	else if (box.n)
		tft_rectangle(0, box.y, BOX_SIZE - box.x, BOX_SIZE, colour);
	else
		tft_rectangle(box.x, box.y, BOX_SIZE, BOX_SIZE, colour);
}

static inline void box_init(void)
{
	timer0_srand();
	box.dx = rand() % 2;
	box.dy = rand() % 2;
	box.n = 0;
	box.e = 1;
	if (game.mode == 0 || game.mode == 1)
		box.x = BOX_INITX;
	else if (game.mode == 2)
		box.x = tft.w - BOX_SIZE / 2;
	else if (game.mode == 3) {
		box.x = BOX_SIZE / 2;
		box.n = 1;
	}
	box.y = BOX_INITY;
	box_show(BOX_FGC);
}

static inline uint8_t box_move(void)
{
#ifdef BOX_TRACE_CLR_CHANGE
	static uint16_t colour = 0x0000;
	box_show(colour);
	colour += 0x0841;
#else
	box_show(BOX_BGC);
#endif
movex:
	if (box.dx) {	// Moving right
		if (box.n) {
			if (box.x == 0)
				box.n = 0;
			else {
				box.x--;
				goto movey;
			}
		}
		switch (game.mode) {
		case 0:
			if (box.x == tft.w - BOX_SIZE) {
				sound_freq(SOUND_EDGE);
				box.dx = 0;
				goto movex;
			}
			break;
		case 2:
			if (box.x < tft.w - BOX_SIZE)
				break;
			if (box.x == tft.w)
				return 1;
			if (!box.e)
				break;
			box.e = 0;
			connect_put(CONN_REPORT);
			if (connect_get() != CONN_READY) {
				game_connFailed();
				return 0;
			}
			connect_put(tft.w - box.x);
			connect_put16(box.y);
			connect_put(box.dy << 1 | box.dx);
			if (connect_get() != CONN_READY) {
				game_connFailed();
				return 0;
			}
			break;
		default:
			if (box.x + BOX_SIZE < bar[1].x)
				break;
			if (box.x + BOX_SIZE == tft.w) {
				game_over(1);
				return 0;
			}
			if (box.x + BOX_SIZE > bar[1].x + bar[1].w)
				break;
			if (box.y + BOX_SIZE >= bar[1].y && \
					box.y <= bar[1].y + bar[1].h) {
				sound_freq(SOUND_TOUCH);
				if (game.mode == 3) {
					connect_put(CONN_SOUND);
					connect_put16(SOUND_TOUCH);
				}
				box.dx = 0;
				box.x = bar[1].x - BOX_SIZE;
				game.score[1]++;
				game_display_score(1);
				goto movex;
			}
		}
		box.x++;
	} else {	// Moving left
		if (game.mode == 3) {
			if (box.x == 0 || (box.n == 1 && box.e)) {
				box.n = 1;
				box.e = 0;
				connect_put(CONN_REPORT);
				if (connect_get() != CONN_READY) {
					game_connFailed();
					return 0;
				}
				connect_put(box.x + 1);
				connect_put16(box.y);
				connect_put(box.dy << 1 | box.dx);
				if (connect_get() != CONN_READY) {
					game_connFailed();
					return 0;
				}
			}
			if (box.n) {
				if (box.x != BOX_SIZE)
					box.x++;
				else
					return 1;
				goto movey;
			}
		} else if (box.x <= bar[0].x + bar[0].w) {
			if (box.x == 0) {
				game_over(0);
				return 0;
			}
			if (box.x >= bar[0].x)
				if (box.y + BOX_SIZE >= bar[0].y && \
				    box.y <= bar[0].y + bar[0].h) {
					sound_freq(SOUND_TOUCH);
					if (game.mode == 2) {
						connect_put(CONN_SOUND);
						connect_put16(SOUND_TOUCH);
					}
					box.dx = 1;
					box.x = bar[0].x + bar[0].w;
					game.score[0]++;
					game_display_score(0);
					goto movex;
				}
		}
		box.x--;
	}
movey:
	if (box.dy) {	// Moving down
		if (box.y == tft.h - BOX_SIZE) {
			sound_freq(SOUND_EDGE);
			if (game.mode == 3 || game.mode == 2) {
				connect_put(CONN_SOUND);
				connect_put16(SOUND_EDGE);
			}
			box.dy = 0;
			goto movey;
		}
		box.y++;
	} else {	// Moving up
		if (box.y == 0) {
			sound_freq(SOUND_EDGE);
			if (game.mode == 3 || game.mode == 2) {
				connect_put(CONN_SOUND);
				connect_put16(SOUND_EDGE);
			}
			box.dy = 1;
			goto movey;
		}
		box.y--;
	}
	box_show(BOX_FGC);
	return 1;
}

#endif
