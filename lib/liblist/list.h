/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef LIST_H
#define LIST_H

#define ITEM_IMAGE_SIZE	(FONT_HEIGHT * 2)

#include <avr/pgmspace.h>
#include <inttypes.h>

struct listItem {
	const char *name;
	const uint8_t *image;
	const listItem **items;
	void (*func)(void);
};

#endif
