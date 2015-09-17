/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef PORTRAITLIST_H
#define PORTRAITLIST_H

#include <tft.h>
#include <rtouch.h>
#include "list.h"

class PortraitList
{
public:
	PortraitList() : curItem(0), pressed(false) {}

	const listItem *currentItem(void) const {return curItem;}
	void setRootItem(listItem *item);
	void display(const listItem *item = 0);
	void refresh(void);
	uint16_t maxScroll(void) const {return max;}
	uint16_t scroll(void) const {return scr;}
	bool setScroll(uint16_t s);
	bool scrollUp(uint16_t s);
	bool scrollDown(uint16_t s);
	uint16_t count(void) const {return cnt;}
	// Not transformed coordinate
	void clickOn(uint16_t x, uint16_t y);
	void toUpperLevel(void);
	void activate(uint16_t index) {activate(*itemsAt(index), true);}
	void activate(const listItem *item, const bool enter = true);
	void pool(rTouch *touch);

private:
	void setCurrentItem(const listItem *item) {curItem = item;}
	void scrollTo(const uint16_t s);
	uint16_t countItems(const listItem **items = 0) const;
	uint16_t itemAt(const uint16_t s, const uint16_t y) const;
	const listItem **itemsAt(const uint16_t index) const;
	void displayTop(void) const;
	// tft->y() specify(index = -1) should not transform
	void displayItem(const listItem *item, const uint16_t index = -1) const;
	void displayItems(const listItem **items, uint16_t index = 0, uint16_t last = 0) const;

	const listItem *curItem;
	const listItem *stack[LIST_STACK_SIZE];
	rTouch::coord_t prev;
	uint16_t scr, max, cnt;
	uint8_t stackSize;
	bool pressed;
};

#endif
