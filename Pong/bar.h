#ifndef BAR_H
#define BAR_H

#include "libili9341/tft.h"
#include "game.h"

#define BAR0_INITH	50
#define BAR0_INITW	3
#define BAR0_INITX	0
#define BAR0_INITY	((tft.h - BAR0_INITH) / 2)
#define BAR0_COLOUR	GREEN

#define BAR1_INITH	50
#define BAR1_INITW	3
#define BAR1_INITX	(tft.w - BAR1_INITW)
#define BAR1_INITY	BAR0_INITY
#define BAR1_COLOUR	CYAN

#define BAR_INTVX	3
#define BAR_INTVY	10

struct {
	uint16_t x, y, c;
	uint8_t w, h;
} bar[2];

#include "box.h"

static inline void bar_show(uint8_t i, uint8_t e)
{
	if (e)
		tft_rectangle(bar[i].x, bar[i].y, \
				bar[i].w, bar[i].h, bar[i].c);
	else
		tft_rectangle(bar[i].x, bar[i].y, \
				bar[i].w, bar[i].h, GAME_BGC);
}

static inline void bar_init(void)
{
	bar[0].x = BAR0_INITX;
	bar[0].y = BAR0_INITY;
	bar[0].w = BAR0_INITW;
	bar[0].h = BAR0_INITH;
	bar[0].c = BAR0_COLOUR;
	bar[1].x = BAR1_INITX;
	bar[1].y = BAR1_INITY;
	bar[1].w = BAR1_INITW;
	bar[1].h = BAR1_INITH;
	bar[1].c = BAR1_COLOUR;
	switch (game.mode) {
	case 1:
		bar_show(1, 1);
	case 0:
	case 2:
		bar_show(0, 1);
		break;
	case 3:
		bar_show(1, 1);
	}
}

static inline void bar_movex(uint8_t i, uint8_t d)
{
	bar_show(i, 0);
	if (d) {
		if (i) {
			if (bar[1].x + bar[1].w < tft.w - BAR_INTVX)
				bar[1].x += BAR_INTVX;
			else
				bar[1].x = tft.w - bar[1].w;
		} else {
			if (bar[0].x + bar[0].w < \
					tft.w / 2 - BOX_SIZE - BAR_INTVX)
				bar[0].x += BAR_INTVX;
			else
				bar[0].x = tft.w / 2 - bar[0].w - BOX_SIZE;
		}
	} else {
		if (i) {
			if (bar[1].x > tft.w / 2 + BOX_SIZE + BAR_INTVX)
				bar[1].x -= BAR_INTVX;
			else
				bar[1].x = tft.w / 2 + BOX_SIZE;
		} else {
			if (bar[0].x > BAR_INTVX)
				bar[0].x -= BAR_INTVX;
			else
				bar[0].x = 0;
		}
	}
	bar_show(i, 1);
}

static inline void bar_movey(uint8_t i, uint8_t d)
{
	bar_show(i, 0);
	if (d) {
		if (bar[i].y + bar[i].h < tft.h - BAR_INTVY)
			bar[i].y += BAR_INTVY;
		else
			bar[i].y = tft.h - bar[i].h;
	} else {
		if (bar[i].y > BAR_INTVY)
			bar[i].y -= BAR_INTVY;
		else
			bar[i].y = 0;
	}
	bar_show(i, 1);
}

static inline void bar_sizew(uint8_t i, uint8_t d)
{
	bar_show(i, 0);
	if (i) {
		if (d)
			bar[1].w += bar[1].w < BAR0_INITW;
		else
			bar[1].w -= bar[1].w > 1;
		if (bar[1].x + bar[1].w > tft.w)
			bar[1].x = tft.w - bar[1].w;
	} else {
		if (d)
			bar[0].w += bar[0].w < BAR0_INITW;
		else
			bar[0].w -= bar[0].w > 1;
		if (bar[0].x + bar[0].w > tft.w / 2 - BOX_SIZE)
			bar[0].x = tft.w / 2 - bar[0].w - BOX_SIZE;
	}
	bar_show(i, 1);
}

static inline void bar_sizeh(uint8_t i, uint8_t d)
{
	bar_show(i, 0);
	if (d) {
		if (i)
			bar[1].h += bar[1].h < BAR1_INITH * 3;
		else
			bar[0].h += bar[0].h < BAR0_INITH * 3;
	} else
		bar[i].h -= bar[i].h > 1;
	if (bar[i].y + bar[i].h > tft.h)
		bar[i].y = tft.h - bar[i].h;
	bar_show(i, 1);
}

static inline void bar_follow(uint8_t i)
{
	bar_show(i, 0);
	if (box.y < bar[i].h / 2)
		bar[i].y = 0;
	else if (box.y > tft.h - bar[i].h / 2)
		bar[i].y = tft.h - bar[i].h;
	else
		bar[i].y = box.y - bar[i].h / 2;
	if (i) {
		if (bar[1].x < box.x + BOX_SIZE) {
			if (box.x + BOX_SIZE > tft.w - bar[1].w)
				bar[1].x = tft.w - bar[1].w;
			else
				bar[1].x = box.x + BOX_SIZE;
		}
	} else {
		if (bar[0].x + bar[0].w > box.x) {
			if (box.x < bar[0].w)
				bar[0].x = 0;
			else
				bar[0].x = box.x - bar[0].w;
		}
	}
	bar_show(i, 1);
}

#endif
