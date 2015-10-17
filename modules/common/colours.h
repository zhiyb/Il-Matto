/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef COLOURS_H
#define COLOURS_H

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

#endif
