#ifndef COLOURS_H
#define COLOURS_H

namespace colour
{
	namespace b4
	{
		enum Colours {Red = 0x04, Green = 0x02, Blue = 0x01, Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue};
	}
	namespace b16
	{
		enum Colours {Red = 0xF800, Green = 0x07E0, Blue = 0x001F, Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue};
	}
	namespace b32
	{
		enum Colours {Red = 0xFF0000, Green = 0x00FF00, Blue = 0x0000FF, Yellow = Red | Green, Cyan = Green | Blue, Magenta = Red | Blue};
	}
}

#endif
