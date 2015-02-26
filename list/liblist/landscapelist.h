#ifndef LANDSCAPELIST_H
#define LANDSCALELIST_H

#include <tft.h>
#include "list.h"

class LandscapeList
{
public:
	LandscapeList(tft_t *tft) : curItem(0) {this->tft = tft;}

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

private:
	uint16_t countItems(const listItem **items = 0) const;
	// Scroll, y-coordinate
	uint16_t itemAt(const uint16_t s, const uint16_t y) const;
	void scrollTo(const uint16_t s);
	// tft->y() specify(index = -1) should not transform
	void displayItem(const listItem *item, const uint16_t index = -1) const;
	void displayItems(const listItem **items, uint16_t index = 0, uint16_t last = 0) const;

	uint16_t scr, max, cnt;
	listItem *curItem;
	tft_t *tft;
};

#endif
