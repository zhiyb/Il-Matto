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
			Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue,
			White = Red | Green | Blue, Black = 0,
		};
	}
	namespace b16
	{
		enum Colours {
			Red = 0xF800, Green = 0x07E0, Blue = 0x001F,
			DarkRed = 0x8000, DarkGreen = 0x0400, DarkBlue = 0x0010,
			Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue,
			DarkYellow = DarkRed | DarkGreen, DarkCyan = DarkGreen | DarkBlue, DarkMagenta = DarkRed | DarkBlue,
			White = Red | Green | Blue, Grey = DarkRed | DarkGreen | DarkBlue, Black = 0,
		};
	}
	namespace b32
	{
		enum Colours {
			Red = 0xFF0000, Green = 0x00FF00, Blue = 0x0000FF,
			DarkRed = 0x800000, DarkGreen = 0x008000, DarkBlue = 0x000080,
			Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue,
			DarkYellow = DarkRed | DarkGreen, DarkCyan = DarkGreen | DarkBlue, DarkMagenta = DarkRed | DarkBlue,
			White = Red | Green | Blue, Grey = DarkRed | DarkGreen | DarkBlue, Black = 0
		};
	}
}

#endif
