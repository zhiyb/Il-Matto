/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef LIST_H
#define LIST_H

// Size of image on each items
#ifndef LIST_ITEM_IMAGE_SIZE
#define LIST_ITEM_IMAGE_SIZE	(FONT_HEIGHT * 2)
#endif

// Size of reserved area at top
#ifndef LIST_TOP_RESERVED
#define LIST_TOP_RESERVED	0
#endif

// Size of reserved area at bottom
#ifndef LIST_BOTTOM_RESERVED
#define LIST_BOTTOM_RESERVED	0
#endif

// Sub menus stack level
#ifndef LIST_STACK_SIZE
#define LIST_STACK_SIZE		5
#endif

#include <avr/pgmspace.h>
#include <inttypes.h>

struct listItem {
	// Display name of this item
	const char *name;
	// Icon of this item
	const uint8_t *image;
	// Children items (sub menu)
	const listItem **items;
	// Function to execute, arg: Enter or leave, ret: Accept
	bool (*func)(bool enter);
};

#endif
