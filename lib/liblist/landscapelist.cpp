/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <rgbconv.h>
#include <colours.h>
#include "landscapelist.h"

#define ZOOM		2
#define TOP_AREA	(tft->topFixedArea())
#define BOTTOM_AREA	(tft->bottomFixedArea())
#define MAX_AREA	(tft->vsMaximum())
#define SCROLL_AREA	(tft->vsHeight())

#define ITEM_PER_COL	4
#define ITEM_SPACE	8
#define ITEM_NAME_X	((ITEM_WIDTH - FONT_WIDTH * ZOOM * 7) / 2)
#define ITEM_NAME_Y	((ITEM_HEIGHT - FONT_HEIGHT * ZOOM))
#define ITEM_IMAGE_X	ITEM_SPACE
#define ITEM_WIDTH	(FONT_WIDTH * ZOOM * 10)
#define ITEM_HEIGHT	(tft->height() / ITEM_PER_COL)
#define ITEM_EMPTY	"*EMPTY*"

#define DEF_TOP_AREA	(ITEM_WIDTH * 0 + 8)
#define DEF_BOTTOM_AREA	(ITEM_WIDTH * 0 + 32)

using namespace colours::b16;

void LandscapeList::refresh(void)
{
	tft->setBackground(Black);
	tft->clean();
	tft->setZoom(ZOOM);
	display();
}

void LandscapeList::display(listItem *item)
{
	tft->setVerticalScrollingArea(DEF_TOP_AREA, DEF_BOTTOM_AREA);
	if (!item) {
		item = currentItem();
		if (!item)
			return;
	} else {
		setCurrentItem(item);
		cnt = countItems();
		max = (count() + ITEM_PER_COL - 1) / ITEM_PER_COL * ITEM_WIDTH;
		if (max > SCROLL_AREA)
			max -= SCROLL_AREA;
		else
			max = 0;
		scr = 0;
	}
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	tft->setTopMask(tft->topEdge());
	tft->setBottomMask(tft->vsMaximum() - tft->bottomEdge());
	if (DEF_TOP_AREA >= ITEM_WIDTH) {
		tft->setXY(0, 0);
		displayItem(currentItem());
	}
	displayItems(currentItem()->items);
}

uint16_t LandscapeList::countItems(const listItem **items) const
{
	if (!items)
		items = currentItem()->items;
	uint16_t i;
	for (i = 0; *items != 0; i++)
		items++;
	return i;
}

void LandscapeList::displayItem(const listItem *item, const uint16_t index) const
{
	uint16_t xs, ys, yt, xt;
	if (index != (uint16_t)-1) {
		xt = index / ITEM_PER_COL * ITEM_WIDTH;		// Relative to top edge
		yt = index % ITEM_PER_COL * ITEM_HEIGHT;
		if (xt + ITEM_WIDTH < scroll())			// Out of scroll area
			return;
		if (xt < scroll()) {				// Above top edge
			xt = tft->topEdge() - (scroll() - xt);
			tft->setX(tft->vsTransform(xt + ITEM_NAME_X));
			goto disp;
		}
		xt -= scroll();					// Relative to top edge
		if (xt >= tft->vsHeight())			// Out of scroll area
			return;
		xt += tft->topEdge();				// Relative to 0, not tramsformed
	} else {
		xt = tft->x();
		yt = tft->y();
	}
	tft->setX(tft->vsTransform(xt + ITEM_NAME_X));

disp:
	xs = tft->vsTransform(xt);
	ys = yt;

	static uint16_t c[] = {Red, Green, Blue, Yellow, Cyan, Magenta};
	tft->setBackground(c[index % (sizeof(c) / sizeof(c[1]))]);
	tft->rectangle(xs, ys, ITEM_WIDTH, ITEM_HEIGHT, tft->background());
	tft->setY(ys + ITEM_NAME_Y);
	if (item) {
		if (item->image)
			tft->drawImage2(item->image, tft->vsTransform(xt + ITEM_IMAGE_X), ys, ITEM_IMAGE_SIZE, ITEM_IMAGE_SIZE, true);
		tft->putString(item->name, true);
#ifdef ITEM_EMPTY
	} else {
		tft->putString(PSTR(ITEM_EMPTY), true);
#endif
	}
}

void LandscapeList::displayItems(const listItem **items, uint16_t index, uint16_t last) const
{
	tft->setTransform(true);
	uint16_t first = itemAt(scroll(), 0);
	if (index < first) {
		for (uint16_t i = index; i < first && *items != 0; i++)
			items++;
		index = first;
	}
	if (!last)
		last = itemAt(scroll() + SCROLL_AREA - 1, tft->height() - 1) + 1;
	while (index < last) {
		displayItem(*items, index++);
		if (*items != 0)
			items++;
	}
	tft->setTransform(false);
}

void LandscapeList::scrollTo(const uint16_t s)
{
	if (!maxScroll())
		scr = 0;
	else if (s >= maxScroll())
		scr = maxScroll() - 1;
	else
		scr = s;
}

void LandscapeList::setScroll(uint16_t s)
{
	if (s == scroll() || (int16_t)s < 0)
		return;
	else if (s > scroll())
		scrollDown(s - scroll());
	else
		scrollUp(scroll() - s);
}

void LandscapeList::scrollDown(uint16_t s)
{
	uint16_t index = itemAt(scroll() + SCROLL_AREA - 1, 0);
	uint16_t scrbak = scroll();
	scrollTo(scroll() + s);
	if ((s = scroll() - scrbak) == 0)
		return;
	tft->setTopMask(s > tft->vsHeight() ? tft->topEdge() : tft->bottomEdge() - s);
	tft->setBottomMask(tft->vsMaximum() - tft->bottomEdge());
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	const listItem **items = curItem->items;
	if (count() <= index)
		items += count();
	else
		items += index;
	displayItems(items, index);
}

void LandscapeList::scrollUp(uint16_t s)
{
	uint16_t last = itemAt(scroll(), 0) + ITEM_PER_COL;
	uint16_t scrbak = scroll();
	scrollTo(scroll() - s);
	if ((s = scrbak - scroll()) == 0)
		return;
	tft->setTopMask(tft->topEdge());
	tft->setBottomMask(tft->vsMaximum() - (s > tft->vsHeight() ? tft->bottomEdge() : tft->topEdge() + s));
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	uint16_t first = itemAt(scroll(), 0);
	displayItems(itemsAt(first), first, last);
}

const listItem **LandscapeList::itemsAt(const uint16_t index)
{
	const listItem **items = curItem->items;
	if (count() <= index)
		items += count();
	else
		items += index;
	return items;
}

uint16_t LandscapeList::itemAt(const uint16_t s, const uint16_t y) const
{
	uint16_t index = s / ITEM_WIDTH * ITEM_PER_COL;
	index += y / ITEM_HEIGHT;
	return index;
}

void LandscapeList::clickOn(uint16_t x, uint16_t y)
{
	if (x < tft->topEdge() || x >= tft->bottomEdge())
		return;
	activate(itemAt(scroll() + x - tft->topEdge(), y));
	return;
}

void LandscapeList::activate(uint16_t index)
{
	const listItem *item = *itemsAt(index);
	if (item->func)
		item->func();
}

void LandscapeList::pool(rTouch *touch)
{
	rTouch::coord_t pos = touch->position();
	switch(touch->status()) {
	case rTouch::Pressed:
		pressed = true;
		break;
	case rTouch::Moved:
		pressed = false;
		if (prev.y > 0 && (int16_t)scroll() - pos.x + prev.x >= 0)
			setScroll((int16_t)scroll() - pos.x + prev.x);
		break;
	case rTouch::Idle:
		if (pressed)
			clickOn(pos.x, pos.y);
		pressed = false;
		break;
	}
#ifdef DEBUG
	using namespace colours::b16;
	tft->rectangle(tft->topEdge() - 1, 0, 1, tft->height(), White);
	tft->rectangle(tft->bottomEdge(), 0, 1, tft->width(), White);
	tft->rectangle(tft->bottomEdge() - 1, 0, 1, tft->width(), Black);
#endif
	prev.x = pos.x;
	prev.y = pos.y;
}
