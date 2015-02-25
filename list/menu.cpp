#include "menu.h"
#include <list.h>

static listItem item[40] = {
	// name, items, parent, func
	{"Item 1", 0, 0, 0},
	{"Item 2", 0, 0, 0},
	{"Item 3", 0, 0, 0},
	{"Item 4", 0, 0, 0},
	{"Item 5", 0, 0, 0},
	{"Item 6", 0, 0, 0},
	{"Item 7", 0, 0, 0},
	{"Item 8", 0, 0, 0},
	{"Item 9", 0, 0, 0},
	{"Item 10", 0, 0, 0},
	{"Item 11", 0, 0, 0},
	{"Item 12", 0, 0, 0},
	{"Item 13", 0, 0, 0},
	{"Item 14", 0, 0, 0},
	{"Item 15", 0, 0, 0},
	{"Item 16", 0, 0, 0},
	{"Item 17", 0, 0, 0},
	{"Item 18", 0, 0, 0},
	{"Item 19", 0, 0, 0},
	{"Item 20", 0, 0, 0},
	{"Item 21", 0, 0, 0},
	{"Item 22", 0, 0, 0},
	{"Item 23", 0, 0, 0},
	{"Item 24", 0, 0, 0},
	{"Item 25", 0, 0, 0},
	{"Item 26", 0, 0, 0},
	{"Item 27", 0, 0, 0},
	{"Item 28", 0, 0, 0},
	{"Item 29", 0, 0, 0},
	{"Item 30", 0, 0, 0},
	{"Item 31", 0, 0, 0},
	{"Item 32", 0, 0, 0},
	{"Item 33", 0, 0, 0},
	{"Item 34", 0, 0, 0},
	{"Item 35", 0, 0, 0},
	{"Item 36", 0, 0, 0},
	{"Item 37", 0, 0, 0},
	{"Item 38", 0, 0, 0},
	{"Item 39", 0, 0, 0},
	{"Item 40", 0, 0, 0},
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
listItem menuRoot = {"Root", rootItems, 0, 0};
