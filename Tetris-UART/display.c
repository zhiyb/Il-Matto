#include <avr/io.h>
#include <avr/eeprom.h>
#include "tft.h"
#include "colour.h"
#include "display.h"
#include "tetris.h"
#include "rgbconv.h"
#include "timer.h"
#include "eemem.h"

// Board area: 150x300, start from 10x10
#define X 10
#define Y 10
#define W 150
#define H 300
// Next unit area: 60x60, start from 170x10
#define XN 170
#define YN 10
#define WN 60
#define HN 60
// Outside frame size, 3 pixel
#define F 3
// Unit size: 15x15
#define U 15
// Unit frame size, 1 pixel
#define UF 1
// Text area: 70x160, start from 165x80
#define XT 165
#define YT 80
#define WT 70
#define HT 160
#define HC 8		// Height of characters
#define HCB (HC * 2)	// Height of big characters
#define WCB (6 * 2)	// Width of big characters
// Text positions
#define YT_TITLE (YT + HC * 0)
#define YT_HIGH (YT_TITLE + HCB + HC * 1)
#define YT_HIGHNUM (YT_HIGH + HC * 1)
#define YT_SCORE (YT_HIGHNUM + HC * 2)
#define YT_SCORENUM (YT_SCORE + HC * 1)
#define YT_SPEED (YT_SCORENUM + HC * 2)
#define YT_SPEEDNUM (YT_SPEED + HC * 1)
#define YT_START (YT_SPEEDNUM + HC * 2)
#define YT_STARTNUM (YT_START + HC * 1)
#define YT_STATE (YT_STARTNUM + HC * 2)
// Save & Load area: Wx(HCB * 6), start from XxYT
#define XSL X
#define YSL YT
#define WSL W
#define HSL (HCB * 6)
// Save & Load text positions
#define XSL_TITLE (XSL + 9)
#define YSL_TITLE (YSL + HCB * 1)
#define XSL_SCORE (XSL + 9)
#define YSL_SCORE (YSL_TITLE + HCB * 1)
#define XSL_ITEM (XSL + 15)
#define YSL_ITEM (YSL_SCORE + HCB * 1)

void Print_saving(uint8_t index)
{
	TFT_putbigs(XSL_ITEM, YSL_ITEM + HCB * index, 2, "- Saving -", \
		Colour_Text_SLSaving, Colour_Text_SLSaving_BG);
}

void Print_failed(uint8_t index)
{
	TFT_putbigs(XSL_ITEM, YSL_ITEM + HCB * index, 2, "- Failed -", \
		Colour_Text_SLFailed, Colour_Text_SLFailed_BG);
}

void Print_saveload(uint8_t index)
{
	if (index != INDEX_INIT)
		goto select;

	// Initialise
	TFT_rectangle(XSL, YSL, WSL, HSL, Colour_SLBackground);

	// Text: Save & Load
	TFT_putbigs(XSL_TITLE, YSL_TITLE, 2, "Save & Load", \
			Colour_Text_SLTitle, Colour_Text_SLTitle_BG);

	// Text: Score
	TFT_putbigs(XSL_SCORE, YSL_SCORE, 2, "Score ", \
			Colour_Text_SLScore, Colour_Text_SLScore_BG);
	if (Tetris_getsavevalid())
		TFT_putbigint16(XSL_SCORE + WCB * 6, YSL_SCORE, 2, \
				Tetris_getsavescore(), \
				Colour_Text_SLScore, Colour_Text_SLScore_BG);
	else
		TFT_putbigs(XSL_SCORE + WCB * 6, YSL_SCORE, 2, "<N/A>",
				Colour_Text_SLScore, Colour_Text_SLScore_BG);
select:
	// Text: Save
	if (index == 0)
		TFT_putbigs(XSL_ITEM, YSL_ITEM, 2, ">> Save <<", \
			Colour_Text_SLSelect, Colour_Text_SLSelect_BG);
	else
		TFT_putbigs(XSL_ITEM, YSL_ITEM, 2, "   Save   ", \
			Colour_Text_SLNot, Colour_Text_SLNot_BG);

	// Text: Load
	if (index == 1)
		TFT_putbigs(XSL_ITEM, YSL_ITEM + HCB * 1, 2, ">> Load <<", \
			Colour_Text_SLSelect, Colour_Text_SLSelect_BG);
	else
		TFT_putbigs(XSL_ITEM, YSL_ITEM + HCB * 1, 2, "   Load   ", \
			Colour_Text_SLNot, Colour_Text_SLNot_BG);
}

void Print_Speed(void)
{
	TFT_rectangle(XT, YT_SPEEDNUM, WT, HC, Colour_Background);
	TFT_putint16(XT, YT_SPEEDNUM, Timer_speed(), \
			Colour_Text_SpeedNum, Colour_Text_SpeedNum_BG);
}

void Print_highest(void)
{
	TFT_rectangle(XT, YT_HIGHNUM, WT, HC, Colour_Background);
	TFT_putint16(XT, YT_HIGHNUM, object.high, \
			Colour_Text_HighNum, Colour_Text_HighNum_BG);
}

void Print_score(void)
{
	TFT_rectangle(XT, YT_SCORENUM, WT, HC, Colour_Background);
	TFT_putint16(XT, YT_SCORENUM, object.score, \
			Colour_Text_ScoreNum, Colour_Text_ScoreNum_BG);
}

void Print_pause(void)
{
	TFT_rectangle(XT, YT_STATE, WT, HC, Colour_Background);
	TFT_puts(XT, YT_STATE, "- PAUSE -", \
			Colour_Text_Pause, Colour_Text_Pause_BG);
}

void Print_start(void)
{
	TFT_rectangle(XT, YT_STATE, WT, HC, Colour_Background);
}

void Print_frame(void)
{
	// Initialise screen
	TFT_fill(Colour_Background);

	// Frame, Outside board area
	TFT_frame(X - F, Y - F, W + F * 2, H + F * 2, F, Colour_Board_Frame);

	// Frame, Outside next unit area
	TFT_frame(XN - F, YN - F, WN + F * 2, HN + F * 2, \
			F, Colour_Next_Frame);

	// Text: title
	TFT_putbigs(XT, YT_TITLE, 2, "Tetris", \
			Colour_Text_Title, Colour_Text_Title_BG);

	// Text: highest score
	TFT_puts(XT, YT_HIGH, "* Highest *", \
			Colour_Text_High, Colour_Text_High_BG);

	// Text: score
	TFT_puts(XT, YT_SCORE, "* Score *", \
			Colour_Text_Score, Colour_Text_Score_BG);

	// Text: speed
	TFT_puts(XT, YT_SPEED, "* Speed *", \
			Colour_Text_Speed, Colour_Text_Speed_BG);

	// Text: startup counter
	TFT_puts(XT, YT_START, "* Startup *", \
			Colour_Text_Start, Colour_Text_Start_BG);
	TFT_putint16(XT, YT_STARTNUM, eeprom_counter(), \
			Colour_Text_StartNum, Colour_Text_StartNum_BG);
}


void Print_gameover(void)
{
	TFT_rectangle(XT, YT_STATE, WT, HC, Colour_Background);
	TFT_puts(XT, YT_STATE, "Gameover!", \
			Colour_Text_Gameover, Colour_Text_Gameover_BG);
}

void Clean_object(void)
{
	uint8_t x, y;
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++) {
			if (object.space[x][y] == Colour_Unit_Empty)
				continue;
			Print_unit(X, Y, object.x + x, object.y + y, \
					Colour_Unit_Empty);
		}
}

void Print_object(void)
{
	uint8_t x, y;
	for (x = 0; x < 4; x++)
		for (y = 0; y < 4; y++) {
			if (object.space[x][y] == Colour_Unit_Empty)
				continue;
			Print_unit(X, Y, object.x + x, object.y + y, \
					object.space[x][y]);
		}
}

void Print_next(void)
{
	uint8_t x, y;

	// Units
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			Print_unit(XN, YN, x, y, next[x][y]);
}

void Print_board(void)
{
	uint8_t x, y;

	// Units
	for (y = 0; y < 20; y++)
		for (x = 0; x < 10; x++)
			Print_unit(X, Y, x, y, board[x][y]);
}

void Print_unit(uint8_t x0, uint16_t y0, uint8_t x, uint8_t y, uint16_t c)
{
	if (c == Colour_Unit_Empty) {
		TFT_rectangle(x0 + x * U + 1, y0 + y * U + 1, U - 2, U - 2, c);
		TFT_frame(x0 + x * U, y0 + y * U, U, U, \
				UF, Colour_Empty_Frame);
		return;
	}
	TFT_rectangle(x0 + x * U + 1, y0 + y * U + 1, U - 2, U - 2, c);
	TFT_frame(x0 + x * U, y0 + y * U, U, U, UF, Colour_Unit_Frame);
}
