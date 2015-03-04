/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef LIST_H
#define LIST_H

// Size of image on each items
#ifndef ITEM_IMAGE_SIZE
#define ITEM_IMAGE_SIZE	(FONT_HEIGHT * 2)
#endif

// Sub menus stack level
#ifndef ITEM_STACK_SIZE
#define ITEM_STACK_SIZE	5
#endif

#include <avr/pgmspace.h>
#include <inttypes.h>

struct listItem {
	const char *name;
	const uint8_t *image;
	const listItem **items;
	bool (*func)(void);
};

#endif
