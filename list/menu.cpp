/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "menu.h"

namespace menu
{

/****************** Miscellaneous item ******************/
namespace misc
{
	static const uint8_t PROGMEM icon_test[] = {
		0x83,0xC1,0x40,0x02,0x20,0x04,0x13,0xC8,0x0C,0x30,0x0C,0x30,0x92,0x49,0x91,0x89,
		0x91,0x89,0x92,0x49,0x0C,0x30,0x0C,0x30,0x13,0xC8,0x20,0x04,0x40,0x02,0x83,0xC1,
	};
	static const char PROGMEM name_03[] = "Item 3";
	static listItem item_03 = {name_03, icon_test, 0, 0};
	static const char PROGMEM name_04[] = "Item 4";
	static listItem item_04 = {name_04, icon_test, 0, 0};
}

namespace toggle
{
	static const char PROGMEM name[] = "LED Toggle";
	static listItem item = {name, misc::icon_test, 0, func};
}

// Diagnosis item
namespace diagnosis
{
	static const char PROGMEM name[] = "Diagnosis";
	static const uint8_t PROGMEM icon[] = {
		0x80,0x01,0x43,0xC2,0x24,0x24,0x08,0x10,0x10,0x08,0x10,0x08,0xD1,0x8B,0x12,0x48,
		0x0A,0x50,0x06,0x60,0x23,0xC4,0x42,0x42,0x83,0xC1,0x02,0x40,0x03,0xC0,0x01,0x80,
	};
	using namespace misc;
	static const listItem *items[] = {
		&toggle::item, &item_03, &item_04,
		&toggle::item, &item_03, &item_04,
		&toggle::item, &item_03, &item_04,
		&toggle::item, &item_03, &item_04,
		&toggle::item, &item_03, &item_04,
		&toggle::item, &item_03, &item_04, 0
	};
	static listItem item = {name, icon, items, 0};
}

// ROOT item
namespace root
{
	static const char PROGMEM name[] = "Root";
	static const uint8_t PROGMEM icon[] = {
		0x00,0x00,0x07,0xE0,0x0F,0xF0,0x1F,0xF8,0x3F,0xFC,0x7F,0xFE,0x7E,0x7E,0x7C,0x3E,
		0x7C,0x3E,0x7E,0x7E,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0,0x07,0xE0,0x00,0x00,
	};
	static const listItem *items[] = {&diagnosis::item, &toggle::item, &misc::item_03, &misc::item_04, 0};
	listItem item = {name, icon, items, 0};
}

}
