#include <avr/eeprom.h>
#include "libili9341/tft.h"
#include "bar.h"
#include "box.h"
#include "timer.h"
#include "rotary.h"
#include "game.h"
#include "connect.h"
#include "sound.h"
#include "eemem.h"

#define ENABLE_CHEAT

struct game_struct game;

uint8_t game_master(void);
void game_slave(void);
void game_mode(void);
void game_init(void);

#define RECTANGLE(r) tft_rectangle(r ## _X, r ## _Y, \
		r ## _W, r ## _H, r ## _BGC)
#define RECTANGLE_CLEAN(r) tft_rectangle(r ## _X, r ## _Y, \
		r ## _W, r ## _H, GAME_BGC)
#define STRING(s) do { \
	tft_setXY(s ## _X, s ## _Y); \
	tft_setColour(s ## _FGC, s ## _BGC); \
	tft_print_string(s); \
} while (0)
#define SELECTION(c, s, a) do { \
	tft_setXY(s ## _X, s ## _Y); \
	if (c) \
		tft_setColour(a ## _ACT_FGC, a ## _ACT_BGC); \
	else \
		tft_setColour(a ## _INACT_FGC, a ## _INACT_BGC); \
	tft_print_string(s); \
} while (0)
#define SOUND(freq) do { \
	sound_freq(freq); \
	connect_put(CONN_SOUND); \
	connect_put16(freq); \
} while (0)

#define GAME_MODE_X		(tft.w / 6)
#define GAME_MODE_Y		(tft.h / 6)
#define GAME_MODE_W		(tft.w * 2 / 3)
#define GAME_MODE_H		(tft.h * 2 / 3)
#define GAME_MODE_BGC		0x2589
#define GAME_MODE_TITLE		"Game mode"
#define GAME_MODE_TITLE_X	(GAME_MODE_X + \
		(GAME_MODE_W - FONT_WIDTH * 2 * 9) / 2)
#define GAME_MODE_TITLE_Y	(GAME_MODE_Y + \
		(GAME_MODE_H - FONT_HEIGHT * 2 * 7) / 2)
#define GAME_MODE_TITLE_FGC	BLUE
#define GAME_MODE_TITLE_BGC	GAME_MODE_BGC
#define GAME_MODE_P1		"1 Player"
#define GAME_MODE_P1_X		GAME_MODE_TITLE_X
#define GAME_MODE_P1_Y		(GAME_MODE_TITLE_Y + FONT_HEIGHT * 2 * 2)
#define GAME_MODE_P2		"2 Players"
#define GAME_MODE_P2_X		GAME_MODE_TITLE_X
#define GAME_MODE_P2_Y		(GAME_MODE_P1_Y + FONT_HEIGHT * 2)
#define GAME_MODE_CONN		"=Connect="
#define GAME_MODE_CONN_X	GAME_MODE_TITLE_X
#define GAME_MODE_CONN_Y	(GAME_MODE_P2_Y + FONT_HEIGHT * 2)
#define GAME_MODE_ORIE		"Orientate"
#define GAME_MODE_ORIE_X	GAME_MODE_TITLE_X
#define GAME_MODE_ORIE_Y	(GAME_MODE_CONN_Y + FONT_HEIGHT * 2 * 2)
#define GAME_MODE_INACT_FGC	RED
#define GAME_MODE_INACT_BGC	GAME_MODE_BGC
#define GAME_MODE_ACT_FGC	BLACK
#define GAME_MODE_ACT_BGC	YELLOW
void game_mode(void)
{
	uint8_t index = 0;
	game.score[0] = 0;
	game.score[1] = 0;
	tft_fill(GAME_BGC);
	while (connect_read() != CONN_NULL);

init:
	RECTANGLE(GAME_MODE);
	tft_setZoom(2);
	STRING(GAME_MODE_TITLE);

list:
	SELECTION(index == 0, GAME_MODE_P1, GAME_MODE);
	SELECTION(index == 1, GAME_MODE_P2, GAME_MODE);
	SELECTION(index == 2, GAME_MODE_CONN, GAME_MODE);
	SELECTION(index == 3, GAME_MODE_ORIE, GAME_MODE);

get:
	switch (ROE_get()) {
	case ROE_SW1:
	case ROE_SW2:
		sound_freq(SOUND_CONF);
		while (ROE_get() != ROE_N);
		goto enter;
	case ROE_CW1:
	case ROE_CW2:
		sound_freq(SOUND_SELECT);
		if (index == 3)
			index = 0;
		else
			index++;
		goto list;
	case ROE_CCW1:
	case ROE_CCW2:
		sound_freq(SOUND_SELECT);
		if (index == 0)
			index = 3;
		else
			index--;
		goto list;
	}
	if (connect_get() == CONN_DETECT) {
		connect_put(CONN_READY);
		game_slave();
		return;
	}
	goto get;

enter:
	game.mode = index;
	if (index == 3) {
		tft_setOrient(tft_getOrient() + 1);
		goto init;
	} else if (index == 2)
		if (!(connect_detect() && game_master())) {
			game_connFailed();
			goto init;
		}
}

#define GAME_M_W	(FONT_WIDTH * 2 * 10 + FONT_WIDTH * 2 * 2)
#define GAME_M_H	(FONT_HEIGHT * 2 * 5 + FONT_HEIGHT * 2 * 2)
#define GAME_M_X	((tft.w - GAME_M_W) / 2)
#define GAME_M_Y	((tft.h - GAME_M_H) / 2)
#define GAME_M_BGC	CYAN
#define GAME_M_STR	"Configure"
#define GAME_M_STR_X	(GAME_M_X + (GAME_M_W - FONT_WIDTH * 2 * 9) / 2)
#define GAME_M_STR_Y	(GAME_M_Y + FONT_HEIGHT * 2 * 1)
#define GAME_M_STR_FGC	BLUE
#define GAME_M_STR_BGC	GAME_M_BGC
#define GAME_M_ORIE	"Orientate"
#define GAME_M_ORIE_X	GAME_M_STR_X
#define GAME_M_ORIE_Y	(GAME_M_STR_Y + FONT_HEIGHT * 2 * 2)
#define GAME_M_LEFT	"Left"
#define GAME_M_LEFT_X	(GAME_M_X + (GAME_M_W - FONT_WIDTH * 2 * 10) / 2)
#define GAME_M_LEFT_Y	(GAME_M_ORIE_Y + FONT_HEIGHT * 2 * 2)
#define GAME_M_RIGHT	"Right"
#define GAME_M_RIGHT_X	(GAME_M_LEFT_X + FONT_WIDTH * 2 * 5)
#define GAME_M_RIGHT_Y	GAME_M_LEFT_Y
#define GAME_M_INACT_FGC	RED
#define GAME_M_INACT_BGC	GAME_M_BGC
#define GAME_M_ACT_FGC		BLACK
#define GAME_M_ACT_BGC		YELLOW
uint8_t game_master(void)
{
	uint8_t index = 0;
	game.mode = 2;

init:
	game_init();
	tft_setZoom(2);
	RECTANGLE(GAME_M);
	STRING(GAME_M_STR);

list:
	SELECTION(index == 0, GAME_M_ORIE, GAME_M);
	SELECTION(index == 1, GAME_M_LEFT, GAME_M);
	SELECTION(index == 2, GAME_M_RIGHT, GAME_M);

get:
	switch (ROE_get()) {
	case ROE_SW1:
	case ROE_SW2:
		SOUND(SOUND_CONF);
		while (ROE_get() != ROE_N);
		goto enter;
	case ROE_CW1:
	case ROE_CW2:
		SOUND(SOUND_SELECT);
		if (index == 2)
			index = 0;
		else
			index++;
refresh:
		if (index == 1 && game.mode == 3) {
			game.mode = 2;
			connect_put(CONN_RIGHT);
			if (connect_get() != CONN_READY)
				goto clean;
			goto init;
		} else if (index == 2 && game.mode == 2) {
			game.mode = 3;
			connect_put(CONN_LEFT);
			if (connect_get() != CONN_READY)
				goto clean;
			goto init;
		}
		goto list;
	case ROE_CCW1:
	case ROE_CCW2:
		SOUND(SOUND_SELECT);
		if (index == 0)
			index = 2;
		else
			index--;
		goto refresh;
	}
	if (connect_read() == CONN_SOUND)
		sound_freq(connect_get16());
	goto get;

enter:
	if (index == 0) {
		tft_setOrient(tft_getOrient() + 1);
		goto init;
	}
	connect_put(CONN_START);
	connect_put(box.dy << 1 | box.dx);
	if (connect_get() != CONN_READY)
		goto clean;
	RECTANGLE_CLEAN(GAME_M);
	box_show(GAME_BGC);
	return 1;

clean:
	tft_fill(GAME_BGC);
	return 0;
}

#define GAME_S_W	(FONT_WIDTH * 2 * 10 + FONT_WIDTH * 2 * 2)
#define GAME_S_H	(FONT_HEIGHT * 2 * 3 + FONT_HEIGHT * 2 * 2)
#define GAME_S_X	((tft.w - GAME_S_W) / 2)
#define GAME_S_Y	((tft.h - GAME_S_H) / 2)
#define GAME_S_BGC	CYAN
#define GAME_S_STR	"Configure"
#define GAME_S_STR_X	(GAME_S_X + (GAME_S_W - FONT_WIDTH * 2 * 9) / 2)
#define GAME_S_STR_Y	(GAME_S_Y + FONT_HEIGHT * 2 * 1)
#define GAME_S_STR_FGC	BLUE
#define GAME_S_STR_BGC	GAME_S_BGC
#define GAME_S_ORIE	"Orientate"
#define GAME_S_ORIE_X	GAME_S_STR_X
#define GAME_S_ORIE_Y	(GAME_S_STR_Y + FONT_HEIGHT * 2 * 2)
#define GAME_S_ORIE_FGC	BLACK
#define GAME_S_ORIE_BGC	YELLOW
void game_slave(void)
{
	game.mode = 3;

init:
	game_init();
	tft_setZoom(2);
	RECTANGLE(GAME_S);
	STRING(GAME_S_STR);
	STRING(GAME_S_ORIE);

get:
	switch (ROE_get()) {
	case ROE_SW1:
	case ROE_SW2:
		SOUND(SOUND_CONF);
		while (ROE_get() != ROE_N);
		tft_setOrient(tft_getOrient() + 1);
		goto init;
	}
	switch (connect_read()) {
	case CONN_LEFT:
		connect_put(CONN_READY);
		game.mode = 2;
		goto init;
	case CONN_DETECT:
	case CONN_RIGHT:
		connect_put(CONN_READY);
		game.mode = 3;
		goto init;
	case CONN_START:
		box.dx = connect_get();
		connect_put(CONN_READY);
		box.dy = (box.dx >> 1) & 0x01;
		box.dx &= 0x01;
		box.e = 0;
		RECTANGLE_CLEAN(GAME_S);
		box_show(GAME_BGC);
		return;
	case CONN_SOUND:
		sound_freq(connect_get16());
	}
	goto get;
}

#define GAME_CONNF_W		(FONT_WIDTH * 2 * 18 + 20)
#define GAME_CONNF_H		(FONT_HEIGHT * 2 * 3)
#define GAME_CONNF_X		((tft.w - GAME_CONNF_W) / 2)
#define GAME_CONNF_Y		((tft.h - GAME_CONNF_H) / 2)
#define GAME_CONNF_BGC		RED
#define GAME_CONNF_STR		"Connection failed!"
#define GAME_CONNF_STR_X	(GAME_CONNF_X + 10)
#define GAME_CONNF_STR_Y	(GAME_CONNF_Y + FONT_HEIGHT * 2)
#define GAME_CONNF_STR_FGC	WHITE
#define GAME_CONNF_STR_BGC	GAME_CONNF_BGC
void game_connFailed(void)
{
	sound_freq(SOUND_FAILED);
	RECTANGLE(GAME_CONNF);
	tft_setZoom(2);
	STRING(GAME_CONNF_STR);
	while (ROE_get() == ROE_N);
	sound_freq(SOUND_CONF);
	while (ROE_get() != ROE_N);
	RECTANGLE_CLEAN(GAME_CONNF);
}

static inline void game_record(uint8_t i)
{
	static uint8_t first = 1;
	if ((first && EEPROM_first()) || i == 2) {
		first = 0;
		eeprom_update_word(EE_PONG_SCORE0, game.score[0]);
		eeprom_update_word(EE_PONG_SCORE1, game.score[1]);
		if (i == 0) {
			eeprom_update_word(EE_PONG_OVERREC0, 1);
			eeprom_update_word(EE_PONG_OVERREC1, 0);
		} else if (i == 1) {
			eeprom_update_word(EE_PONG_OVERREC0, 0);
			eeprom_update_word(EE_PONG_OVERREC1, 1);
		} else if (i == 2) {
			eeprom_update_word(EE_PONG_OVERREC0, 0);
			eeprom_update_word(EE_PONG_OVERREC1, 0);
		}
	} else {
		if (eeprom_read_word(EE_PONG_SCORE0) < game.score[0])
			eeprom_update_word(EE_PONG_SCORE0, game.score[0]);
		if (eeprom_read_word(EE_PONG_SCORE1) < game.score[1])
			eeprom_update_word(EE_PONG_SCORE1, game.score[1]);
		if (i == 0)
			eeprom_update_word(EE_PONG_OVERREC0, \
				eeprom_read_word(EE_PONG_OVERREC0) + 1);
		else
			eeprom_update_word(EE_PONG_OVERREC1, \
				eeprom_read_word(EE_PONG_OVERREC1) + 1);
	};
}

#define GAME_OVER_W		(FONT_WIDTH * 2 * 12 + FONT_WIDTH * 2 * 2)
#define GAME_OVER_H		(FONT_HEIGHT * 2 * 5 + FONT_HEIGHT * 2 * 2)
#define GAME_OVER_X		((tft.w - GAME_OVER_W) / 2)
#define GAME_OVER_Y		((tft.h - GAME_OVER_H) / 2)
#define GAME_OVER_BGC		LTY
#define GAME_OVER_STR		"GAME OVER!"
#define GAME_OVER_STR_X		(GAME_OVER_X + \
		(GAME_OVER_W - FONT_WIDTH * 2 * 10) / 2)
#define GAME_OVER_STR_Y		(GAME_OVER_Y + FONT_HEIGHT * 2)
#define GAME_OVER_STR_FGC	RED
#define GAME_OVER_STR_BGC	GAME_OVER_BGC
#define GAME_OVER_LOST_X	(GAME_OVER_X + \
		(GAME_OVER_W - FONT_WIDTH * 2 * 12) / 2)
#define GAME_OVER_LOST_Y	(GAME_OVER_STR_Y + FONT_HEIGHT * 2)
#define GAME_OVER_LOST_FGC	BLACK
#define GAME_OVER_LOST_BGC	GAME_OVER_BGC
#define GAME_OVER_HIGH_X	(GAME_OVER_X + \
		(GAME_OVER_W - FONT_WIDTH * 2 * 11) / 2)
#define GAME_OVER_HIGH_Y	(GAME_OVER_LOST_Y + FONT_HEIGHT * 2)
#define GAME_OVER_HIGH_FGC	GREEN
#define GAME_OVER_HIGH_BGC	GAME_OVER_BGC
#define GAME_OVER_REC_X		(GAME_OVER_X + \
		(GAME_OVER_W - FONT_WIDTH * 2 * 11) / 2)
#define GAME_OVER_REC_Y		(GAME_OVER_HIGH_Y + FONT_HEIGHT * 2)
#define GAME_OVER_REC_FGC	BLUE
#define GAME_OVER_REC_BGC	GAME_OVER_BGC
#define GAME_OVER_CLN		"Clean record"
#define GAME_OVER_CLN_X		(GAME_OVER_X + \
		(GAME_OVER_W - FONT_WIDTH * 2 * 12) / 2)
#define GAME_OVER_CLN_Y		(GAME_OVER_REC_Y + FONT_HEIGHT * 2)
#define GAME_OVER_CLN_FGC	BLACK
#define GAME_OVER_CLN_BGC	YELLOW
void game_over(uint8_t i)
{
	if (game.mode == 2 || game.mode == 3) {
		connect_put(CONN_GAMEOVER);
		connect_put(i);
		if (connect_get() != CONN_READY) {
			game_connFailed();
			return;
		}
	}
	sound_freq(SOUND_FAILED);
	game_record(i);
	RECTANGLE(GAME_OVER);
	tft_setZoom(2);
	STRING(GAME_OVER_STR);
	tft_setXY(GAME_OVER_LOST_X, GAME_OVER_LOST_Y);
	tft_setColour(GAME_OVER_LOST_FGC, GAME_OVER_LOST_BGC);
	tft_print_string("Player");
	tft_print_uint16(i + 1);
	tft_print_string(" LOST");
	tft_setXY(GAME_OVER_HIGH_X, GAME_OVER_HIGH_Y);
	tft_setColour(GAME_OVER_HIGH_FGC, GAME_OVER_HIGH_BGC);
	tft_print_uint16(eeprom_read_word(EE_PONG_SCORE0));
	tft_print_string("/");
	tft_print_uint16(eeprom_read_word(EE_PONG_SCORE1));
	tft_setXY(GAME_OVER_REC_X, GAME_OVER_REC_Y);
	tft_setColour(GAME_OVER_REC_FGC, GAME_OVER_REC_BGC);
	tft_print_uint16(eeprom_read_word(EE_PONG_OVERREC0));
	tft_print_string("/");
	tft_print_uint16(eeprom_read_word(EE_PONG_OVERREC1));
	STRING(GAME_OVER_CLN);
get:
	switch (ROE_get()) {
	case ROE_SW2:
	case ROE_SW1:
		game_record(2);
		sound_freq(SOUND_CONF);
		break;
	case ROE_N:
		goto get;
	}
	sound_freq(SOUND_CANCEL);
	while (ROE_get() != ROE_N);
}

#define GAME_SCORE0_X	20
#define GAME_SCORE0_Y	20
#define GAME_SCORE0_FGC	BAR0_COLOUR
#define GAME_SCORE0_BGC	GAME_BGC
#define GAME_SCORE1_X	(tft.w - 20 - FONT_WIDTH * 2 * 5)
#define GAME_SCORE1_Y	20
#define GAME_SCORE1_FGC	BAR1_COLOUR
#define GAME_SCORE1_BGC	GAME_BGC
void game_display_score(uint8_t i)
{
	if (i) {
		tft_setXY(GAME_SCORE1_X, GAME_SCORE1_Y);
		tft_setColour(GAME_SCORE1_FGC, GAME_SCORE1_BGC);
	} else {
		tft_setXY(GAME_SCORE0_X, GAME_SCORE0_Y);
		tft_setColour(GAME_SCORE0_FGC, GAME_SCORE0_BGC);
	}
	tft_setZoom(2);
	tft_print_uint16(game.score[i]);
}

void game_p1(void)
{
	uint8_t cheat = 0, mode = 0;
get:
	switch (ROE_get()) {
	case ROE_CW1:
		if (mode)
			bar_sizeh(0, 1);
		else if (!cheat)
			bar_movey(0, 1);
		break;
	case ROE_CCW1:
		if (mode)
			bar_sizeh(0, 0);
		else if (!cheat)
			bar_movey(0, 0);
		break;
	case ROE_CW2:
		if (mode)
			bar_sizew(0, 1);
		else if (!cheat)
			bar_movex(0, 1);
		break;
	case ROE_CCW2:
		if (mode)
			bar_sizew(0, 0);
		else if (!cheat)
			bar_movex(0, 0);
		break;
	case ROE_SW1:
		while (ROE_get() == ROE_SW1);
#ifdef ENABLE_CHEAT
		cheat = !cheat;
#else
		mode = !mode;
#endif
		break;
	case ROE_SW2:
		while (ROE_get() == ROE_SW2);
		mode = !mode;
		break;
	default:
		if (!mode && timer0()) {
			if (cheat)
				bar_follow(0);
			if (!box_move())
				return;
		}
	}
	goto get;
}

void game_p2(void)
{
	uint8_t cheat[2] = {0, 0};
get:
	switch (ROE_get()) {
	case ROE_CW1:
		if (!cheat[0])
			bar_movey(0, 1);
		break;
	case ROE_CCW1:
		if (!cheat[0])
			bar_movey(0, 0);
		break;
	case ROE_CW2:
		if (!cheat[1])
			bar_movey(1, 1);
		break;
	case ROE_CCW2:
		if (!cheat[1])
			bar_movey(1, 0);
		break;
	case ROE_SW1:
		while (ROE_get() == ROE_SW1);
#ifdef ENABLE_CHEAT
		cheat[0] = !cheat[0];
#endif
		break;
	case ROE_SW2:
		while (ROE_get() == ROE_SW2);
#ifdef ENABLE_CHEAT
		cheat[1] = !cheat[1];
#endif
		break;
	default:
		if (timer0()) {
			if (cheat[0])
				bar_follow(0);
			if (cheat[1])
				bar_follow(1);
			if (!box_move())
				return;
		}
	}
	goto get;
}

void game_conn2(void)
{
	uint8_t cheat = 0;

get:
	switch (ROE_get()) {
	case ROE_CW1:
	case ROE_CW2:
		if (!cheat)
			bar_movey(0, 1);
		break;
	case ROE_CCW1:
	case ROE_CCW2:
		if (!cheat)
			bar_movey(0, 0);
		break;
	case ROE_SW1:
	case ROE_SW2:
		while (ROE_get() != ROE_N);
		if (!box.e)
			return;
#ifdef ENABLE_CHEAT
		cheat = !cheat;
#endif
		break;
	default:
		if (timer0()) {
			if (cheat && box.e)
				bar_follow(0);
			if (!box_move())
				return;
		}
	}
	switch (connect_read()) {
	case CONN_REPORT:
		connect_put(CONN_READY);
		box.x = connect_get();
		box.y = connect_get16();
		box.dx = connect_get();
		connect_put(CONN_READY);
		box.x = tft.w - box.x;
		if (box.y + BOX_SIZE > tft.h)
			box.y = tft.h - BOX_SIZE;
		box.dy = (box.dx >> 1) & 0x01;
		box.dx &= 0x01;
		box.e = 1;
		break;
	case CONN_GAMEOVER:
		cheat = connect_get();
		connect_put(CONN_READY);
		game.mode = 1;		// Disable game_over() send function
		game_over(cheat);
		return;
	case CONN_SOUND:
		sound_freq(connect_get16());
	}
	goto get;
}

void game_conn3(void)
{
	uint8_t cheat = 0;

get:
	switch (ROE_get()) {
	case ROE_CW1:
	case ROE_CW2:
		if (!cheat)
			bar_movey(1, 1);
		break;
	case ROE_CCW1:
	case ROE_CCW2:
		if (!cheat)
			bar_movey(1, 0);
		break;
	case ROE_SW1:
	case ROE_SW2:
		while (ROE_get() != ROE_N);
		if (!box.e)
			return;
#ifdef ENABLE_CHEAT
		cheat = !cheat;
#endif
		break;
	default:
		if (timer0()) {
			if (cheat && box.e)
				bar_follow(1);
			if (!box_move())
				return;
		}
	}
	switch (connect_read()) {
	case CONN_REPORT:
		connect_put(CONN_READY);
		box.x = connect_get();
		box.y = connect_get16();
		box.dx = connect_get();
		connect_put(CONN_READY);
		if (box.y + BOX_SIZE > tft.h)
			box.y = tft.h - BOX_SIZE;
		box.dy = (box.dx >> 1) & 0x01;
		box.dx &= 0x01;
		box.n = 1;
		box.e = 1;
		break;
	case CONN_GAMEOVER:
		cheat = connect_get();
		connect_put(CONN_READY);
		game.mode = 1;		// Disable game_over() send function
		game_over(cheat);
		return;
	case CONN_SOUND:
		sound_freq(connect_get16());
	}
	goto get;
}

void game_init(void)
{
	tft_fill(GAME_BGC);
	bar_init();
	box_init();
	switch (game.mode) {
	case 1:
		game_display_score(1);
	case 0:
	case 2:
		game_display_score(0);
		break;
	case 3:
		game_display_score(1);
	}
}

void game_main(void)
{
	game_mode();
	switch (game.mode) {
	case 0:
		game_init();
		game_p1();
		break;
	case 1:
		game_init();
		game_p2();
		break;
	case 2:
		game_conn2();
		break;
	case 3:
		game_conn3();
		break;
	}
}
