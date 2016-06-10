/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef COLOURS_H
#define COLOURS_H

#include <stdint.h>

// Combine RGB/HSV components
#define COLOUR_888(r, g, b)	((((uint32_t)(r) & 0xff) << 16) | \
				(((uint32_t)(g) & 0xff) << 8) | \
				(((uint32_t)(b) & 0xff) << 0))
#define COLOUR_565(r, g, b)	((((uint16_t)(r) & 0xf8) << 8) | \
				(((uint16_t)(g) & 0xfc) << 3) | \
				(((uint16_t)(b) & 0xf8) >> 3))
// Convert between RGB-888 and RGB-565
#define COLOUR_888_TO_565(c)	((((uint32_t)(c) & 0x00f80000) >> 8) | \
				(((uint32_t)(c) & 0x0000fc00) >> 5) | \
				(((uint32_t)(c) & 0x000000f8) >> 3))
// TODO #define COLOUR_565_TO_888(c)	()
// Invert RGB & BGR
#define COLOUR_888_I(c)	((((uint32_t)(c) & 0x00ff0000) >> 16) | \
			((uint32_t)(c) & 0x0000ff00) | \
			(((uint32_t)(c) & 0x000000ff) << 16))
#define COLOUR_565_I(c)	(((((uint16_t)(c) & 0xf800) >> 11) | \
			((uint16_t)(c) & 0x07e0) | \
			(((uint16_t)(c) & 0x001f) << 11))
// Extract RGB components
#define RED_888(c)	(((uint32_t)(c) >> 16) & 0xff)
#define GREEN_888(c)	(((uint32_t)(c) >> 8) & 0xff)
#define BLUE_888(c)	(((uint32_t)(c) >> 0) & 0xff)
#define RED_565(c)	(((uint16_t)(c) >> 8) & 0xff)
#define GREEN_565(c)	(((uint16_t)(c) >> 3) & 0xff)
#define BLUE_565(c)	(((uint16_t)(c) << 3) & 0xff)
#define H_888(c)	(RED_888(c))
#define S_888(c)	(GREEN_888(c))
#define V_888(c)	(BLUE_888(c))

#ifdef __cplusplus
extern "C" {
#endif

uint32_t colour_hsv_to_rgb(uint32_t hsv);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace colours
{
namespace b4
{
	enum Colours {
		Red = 0x04, Green = 0x02, Blue = 0x01,
		Yellow	= Red	| Green,
		Cyan	= Green	| Blue,
		Magenta	= Red	| Blue,
		White = Red | Green | Blue, Black = 0,
	};
}
namespace b16
{
	enum Colours {
		Red = 0xf800, Green = 0x07e0, Blue = 0x001f,
		DarkRed = 0x7800, DarkGreen = 0x03e0, DarkBlue = 0x000f,

		Yellow	= Red | Green,
		Cyan	= Green | Blue,
		Magenta	= Red | Blue,
		DarkYellow	= DarkRed | DarkGreen,
		DarkCyan	= DarkGreen | DarkBlue,
		DarkMagenta	= DarkRed | DarkBlue,

		LightRed = Red | DarkCyan,
		LightGreen = Green | DarkMagenta,
		LightBlue = Blue | DarkYellow,
		LightYellow = Yellow | DarkBlue,
		LightCyan = Cyan | DarkRed,
		LightMagenta = Magenta | DarkGreen,

		White = Red | Green | Blue,
		Grey = DarkRed | DarkGreen | DarkBlue,
		Black = 0,

		Orange = Red | DarkGreen,
		Chartreuse = DarkRed | Green,
		SpringGreen = Green | DarkBlue,
		Azure = DarkGreen | Blue,
		Pink = Red | DarkBlue,
		Violet = DarkRed | Blue,
	};
}
namespace b32
{
	enum Colours {
		Red = 0xff0000, Green = 0x00ff00, Blue = 0x0000ff,
		DarkRed = 0x7f0000, DarkGreen = 0x007f00, DarkBlue = 0x00007f,

		Yellow	= Red | Green,
		Cyan	= Green | Blue,
		Magenta	= Red | Blue,
		DarkYellow	= DarkRed | DarkGreen,
		DarkCyan	= DarkGreen | DarkBlue,
		DarkMagenta	= DarkRed | DarkBlue,

		LightRed = Red | DarkCyan,
		LightGreen = Green | DarkMagenta,
		LightBlue = Blue | DarkYellow,
		LightYellow = Yellow | DarkBlue,
		LightCyan = Cyan | DarkRed,
		LightMagenta = Magenta | DarkGreen,

		White = Red | Green | Blue,
		Grey = DarkRed | DarkGreen | DarkBlue,
		Black = 0,

		Orange = Red | DarkGreen,
		Chartreuse = DarkRed | Green,
		SpringGreen = Green | DarkBlue,
		Azure = DarkGreen | Blue,
		Pink = Red | DarkBlue,
		Violet = DarkRed | Blue,
	};
}
}
#else
#define Red		0xff0000
#define Green		0x00ff00
#define Blue		0x0000ff
#define DarkRed		0x7f0000
#define DarkGreen	0x007f00
#define DarkBlue	0x00007f

#define Yellow		(Red | Green)
#define Cyan		(Green | Blue)
#define Magenta		(Red | Blue)
#define DarkYellow	(DarkRed | DarkGreen)
#define DarkCyan	(DarkGreen | DarkBlue)
#define DarkMagenta	(DarkRed | DarkBlue)

#define LightRed	(Red | DarkCyan)
#define LightGreen	(Green | DarkMagenta)
#define LightBlue	(Blue | DarkYellow)
#define LightYellow	(Yellow | DarkBlue)
#define LightCyan	(Cyan | DarkRed)
#define LightMagenta	(Magenta | DarkGreen)

#define White		(Red | Green | Blue)
#define Grey		(DarkRed | DarkGreen | DarkBlue)
#define Black		0

#define Orange		(Red | DarkGreen)
#define Chartreuse	(DarkRed | Green)
#define SpringGreen	(Green | DarkBlue)
#define Azure		(DarkGreen | Blue)
#define Pink		(Red | DarkBlue)
#define Violet		(DarkRed | Blue)
#endif

#endif
