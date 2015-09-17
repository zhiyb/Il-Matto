/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef LANDSCAPELIST_H
#define LANDSCALELIST_H

#include <tft.h>
#include <rtouch.h>
#include "list.h"

class LandscapeList
{
public:
	LandscapeList() : curItem(0), pressed(false) {}

	listItem *currentItem(void) const {return curItem;}
	void setCurrentItem(listItem *item) {curItem = item;}
	void refresh(void);
	void display(listItem *item = 0);
	uint16_t maxScroll(void) const {return max;}
	uint16_t scroll(void) const {return scr;}
	void setScroll(uint16_t s);
	void scrollUp(uint16_t s);
	void scrollDown(uint16_t s);
	uint16_t count(void) const {return cnt;}
	// Not transformed coordinate
	void clickOn(uint16_t x, uint16_t y);
	void activate(uint16_t index);
	void pool(rTouch *touch);

private:
	void scrollTo(const uint16_t s);
	uint16_t countItems(const listItem **items = 0) const;
	uint16_t itemAt(const uint16_t s, const uint16_t y) const;
	const listItem **itemsAt(const uint16_t index);
	// tft->x() specify(index = -1) should not transform
	void displayItem(const listItem *item, const uint16_t index = -1) const;
	void displayItems(const listItem **items, uint16_t index = 0, uint16_t last = 0) const;

	listItem *curItem;
	bool pressed;
	rTouch::coord_t prev;
	uint16_t scr, max, cnt;
};

#endif
