/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <list.h>
#include "menu.h"

static void onLED(void)
{
	PORTB |= _BV(7);
}

static void toggleLED(void)
{
	PINB |= _BV(7);
}

static void offLED(void)
{
	PORTB &= ~_BV(7);
}

static void onLED2(void)
{
	PORTD |= _BV(6);
}

static void toggleLED2(void)
{
	PIND |= _BV(6);
}

static void offLED2(void)
{
	PORTD &= ~_BV(6);
}

const char PROGMEM item01[] = "Item 1";
const char PROGMEM item02[] = "LED ON";
const char PROGMEM item03[] = "Item 3";
const char PROGMEM item04[] = "Item 4";
const char PROGMEM item05[] = "Item 5";
const char PROGMEM item06[] = "LED Toggle";
const char PROGMEM item07[] = "Item 7";
const char PROGMEM item08[] = "Item 8";
const char PROGMEM item09[] = "LED2 ON";
const char PROGMEM item10[] = "Item 10";
const char PROGMEM item11[] = "Item 11";
const char PROGMEM item12[] = "Item 12";
const char PROGMEM item13[] = "LED2 Toggle";
const char PROGMEM item14[] = "Item 14";
const char PROGMEM item15[] = "Item 15";
const char PROGMEM item16[] = "LED2 OFF";
const char PROGMEM item17[] = "Item 17";
const char PROGMEM item18[] = "Item 18";
const char PROGMEM item19[] = "Item 19";
const char PROGMEM item20[] = "LED OFF";
const char PROGMEM item21[] = "Item 21";
const char PROGMEM item22[] = "Item 22";
const char PROGMEM item23[] = "Item 23";
const char PROGMEM item24[] = "Item 24";
const char PROGMEM item25[] = "Item 25";
const char PROGMEM item26[] = "Item 26";
const char PROGMEM item27[] = "Item 27";
const char PROGMEM item28[] = "Item 28";
const char PROGMEM item29[] = "Item 29";
const char PROGMEM item30[] = "Item 30";
const char PROGMEM item31[] = "Item 31";
const char PROGMEM item32[] = "Item 32";
const char PROGMEM item33[] = "Item 33";
const char PROGMEM item34[] = "Item 34";
const char PROGMEM item35[] = "Item 35";
const char PROGMEM item36[] = "Item 36";
const char PROGMEM item37[] = "Item 37";
const char PROGMEM item38[] = "Item 38";
const char PROGMEM item39[] = "Item 39";
const char PROGMEM item40[] = "Item 40";

static listItem item[40] = {
	// name, items, parent, func
	{item01, 0, 0, 0},
	{item02, 0, 0, onLED},
	{item03, 0, 0, 0},
	{item04, 0, 0, 0},
	{item05, 0, 0, 0},
	{item06, 0, 0, toggleLED},
	{item07, 0, 0, 0},
	{item08, 0, 0, 0},
	{item09, 0, 0, onLED2},
	{item10, 0, 0, 0},
	{item11, 0, 0, 0},
	{item12, 0, 0, 0},
	{item13, 0, 0, toggleLED2},
	{item14, 0, 0, 0},
	{item15, 0, 0, 0},
	{item16, 0, 0, offLED2},
	{item17, 0, 0, 0},
	{item18, 0, 0, 0},
	{item19, 0, 0, 0},
	{item20, 0, 0, offLED},
	{item21, 0, 0, 0},
	{item22, 0, 0, 0},
	{item23, 0, 0, 0},
	{item24, 0, 0, 0},
	{item25, 0, 0, 0},
	{item26, 0, 0, 0},
	{item27, 0, 0, 0},
	{item28, 0, 0, 0},
	{item29, 0, 0, 0},
	{item30, 0, 0, 0},
	{item31, 0, 0, 0},
	{item32, 0, 0, 0},
	{item33, 0, 0, 0},
	{item34, 0, 0, 0},
	{item35, 0, 0, 0},
	{item36, 0, 0, 0},
	{item37, 0, 0, 0},
	{item38, 0, 0, 0},
	{item39, 0, 0, 0},
	{item40, 0, 0, 0},
};

static const listItem *rootItems[] = {
	&item[0], &item[1], &item[2], &item[3],
	&item[4], &item[5], &item[6], &item[7],
	&item[8], &item[9], &item[10], &item[11],
	&item[12], &item[13], &item[14], &item[15],
	&item[16], &item[17], &item[18], &item[19],
	&item[20], &item[21], &item[22], &item[23],
	&item[24], &item[25], &item[26], &item[27],
	&item[28], &item[29], &item[30], &item[31],
	&item[32], &item[33], &item[34], &item[35],
	&item[36], &item[37], &item[38], &item[39],
	0};

// name, items, parent, func
const char PROGMEM rootName[] = "Root";
listItem menuRoot = {rootName, rootItems, 0, 0};
