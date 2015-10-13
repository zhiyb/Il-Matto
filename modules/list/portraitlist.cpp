/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#include <colours.h>
#include <tft.h>
#include "portraitlist.h"

#define ZOOM		2
#define TOP_AREA	(tft::topFixedArea())
#define BOTTOM_AREA	(tft::bottomFixedArea())
#define MAX_AREA	(tft::vsMaximum())
#define SCROLL_AREA	(tft::vsHeight())

#define ITEM_SPACE	8
#define ITEM_NAME_X	(ITEM_IMAGE_X + LIST_ITEM_IMAGE_SIZE + ITEM_SPACE)
#define ITEM_NAME_Y	8
#define ITEM_IMAGE_X	ITEM_SPACE
#define ITEM_HEIGHT	(FONT_HEIGHT * ZOOM + ITEM_NAME_Y * 2)
//#define ITEM_EMPTY	"** EMPTY **"

#define DEF_TOP_AREA	(ITEM_HEIGHT * 1 + LIST_TOP_RESERVED)
#define DEF_BOTTOM_AREA	(ITEM_HEIGHT * 0 + LIST_BOTTOM_RESERVED)

using namespace colours::b16;

void PortraitList::setRootItem(listItem *item)
{
	setCurrentItem(item);
	stackSize = 0;
}

void PortraitList::refresh(void)
{
	tft::background = Black;
	tft::zoom = ZOOM;
	tft::clean();
	display();
}

void PortraitList::display(const listItem *item)
{
	tft::setVerticalScrollingArea(DEF_TOP_AREA, DEF_BOTTOM_AREA);
	if (!item) {
		if (!(item = currentItem()))
			return;
	} else {
		setCurrentItem(item);
		cnt = countItems();
		max = count() * ITEM_HEIGHT;
		if (max > SCROLL_AREA)
			max -= SCROLL_AREA;
		else
			max = 0;
		scr = 0;
	}
	tft::setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	displayTop();
	tft::setTopMask(tft::topEdge());
	tft::setBottomMask(tft::vsMaximum() - tft::bottomEdge());
	displayItems(currentItem()->items);
}

uint16_t PortraitList::countItems(const listItem **items) const
{
	if (!items)
		items = currentItem()->items;
	uint16_t i;
	for (i = 0; *items != 0; i++)
		items++;
	return i;
}

void PortraitList::displayTop(void) const
{
	tft::setTopMask(0);
	tft::setBottomMask(tft::vsMaximum() - tft::topEdge());
	tft::y = DEF_TOP_AREA - ITEM_HEIGHT;
	tft::foreground = Black;
	tft::zoom = ZOOM;
	tft::setTransform(false);
	displayItem(currentItem());
	tft::frame(0, DEF_TOP_AREA - ITEM_HEIGHT, tft::width, ITEM_HEIGHT, ZOOM, Grey);
}

void PortraitList::displayItem(const listItem *item, const uint16_t index) const
{
	uint16_t ys = tft::y;
	if (index != (uint16_t)-1) {
		uint16_t y = index * ITEM_HEIGHT;		// Relative to TOP_AREA
		if (y + ITEM_HEIGHT < scroll())			// Out of scroll area
			return;
		if (y < scroll()) {				// Above TOP_AREA
			ys = TOP_AREA - (scroll() - y);
			tft::y = tft::vsTransform(ys + ITEM_NAME_Y);
			goto disp;
		}
		y -= scroll();					// Relative to upperEdge
		if (y >= tft::vsHeight())			// Out of scroll area
			return;
		ys = tft::vsTransform(y + tft::topEdge());
		tft::y = tft::vsTransform(y + tft::topEdge() + ITEM_NAME_Y);
	} else
		tft::y = tft::vsTransform(tft::y) + ITEM_NAME_Y;

	static uint16_t c[] = {Red, Green, Blue, Yellow, Cyan, Magenta};
disp:
	if (index == (uint16_t)-1)
		tft::background = White;
	else
		tft::background = c[index % (sizeof(c) / sizeof(c[1]))];
	tft::rectangle(0, ys, tft::width, ITEM_HEIGHT, tft::background);
	tft::x = ITEM_NAME_X;
	if (item) {
		if (item->image)
			tft::drawImage2(item->image, ITEM_IMAGE_X, tft::y, LIST_ITEM_IMAGE_SIZE, LIST_ITEM_IMAGE_SIZE, true);
		tft::putString(item->name, true);
#ifdef ITEM_EMPTY
	} else {
		tft::putString(PSTR(ITEM_EMPTY), true);
#endif
	}
}

void PortraitList::displayItems(const listItem **items, uint16_t index, uint16_t last) const
{
	tft::foreground = Black;
	tft::zoom = ZOOM;
	tft::setTransform(true);
	uint16_t first = itemAt(scroll(), 0);
	if (index < first) {
		for (uint16_t i = index; i < first && *items != 0; i++)
			items++;
		index = first;
	}
	if (!last)
		last = itemAt(scroll() + SCROLL_AREA - 1, tft::width - 1) + 1;
	while (index < last) {
		displayItem(*items, index++);
		if (*items != 0)
			items++;
	}
	tft::setTransform(false);
}

void PortraitList::scrollTo(const uint16_t s)
{
	if (!maxScroll())
		scr = 0;
	else if (s >= maxScroll())
		scr = maxScroll() - 1;
	else
		scr = s;
}

bool PortraitList::setScroll(uint16_t s)
{
	if (s == scroll() || (int16_t)s < 0)
		return false;
	else if (s > scroll())
		return scrollDown(s - scroll());
	else
		return scrollUp(scroll() - s);
}

bool PortraitList::scrollDown(uint16_t s)
{
	uint16_t index = itemAt(scroll() + SCROLL_AREA - 1, 0);
	uint16_t scrbak = scroll();
	scrollTo(scroll() + s);
	if ((s = scroll() - scrbak) == 0)
		return false;
	tft::setTopMask(s > tft::vsHeight() ? tft::topEdge() : tft::bottomEdge() - s);
	tft::setBottomMask(tft::vsMaximum() - tft::bottomEdge());
	tft::setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	const listItem **items = curItem->items;
	if (count() <= index)
		items += count();
	else
		items += index;
	displayItems(items, index);
	return true;
}

bool PortraitList::scrollUp(uint16_t s)
{
	uint16_t last = itemAt(scroll(), 0) + 1;
	uint16_t scrbak = scroll();
	scrollTo(scroll() - s);
	if ((s = scrbak - scroll()) == 0)
		return false;
	tft::setTopMask(tft::topEdge());
	tft::setBottomMask(tft::vsMaximum() - (s > tft::vsHeight() ? tft::bottomEdge() : tft::topEdge() + s));
	tft::setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	uint16_t first = itemAt(scroll(), 0);
	displayItems(itemsAt(first), first, last);
	return true;
}

const listItem **PortraitList::itemsAt(const uint16_t index) const
{
	const listItem **items = curItem->items;
	if (count() <= index)
		items += count();
	else
		items += index;
	return items;
}

uint16_t PortraitList::itemAt(uint16_t s, uint16_t x) const
{
	uint16_t index = s / ITEM_HEIGHT;
	return index;
}

void PortraitList::toUpperLevel(void)
{
	if (!stackSize)
		return;
	display(stack[--stackSize]);
}

void PortraitList::clickOn(uint16_t x, uint16_t y)
{
	if (y >= tft::bottomEdge())
		return;
	if (y < tft::topEdge()) {
		activate(currentItem(), false);
		return;
	}
	activate(itemAt(scroll() + y - tft::topEdge(), x));
	return;
}

void PortraitList::activate(const listItem *item, const bool enter)
{
	if (!item)
		return;
	tft::setTransform(true);
	bool accept = true;
	if (item->func)
		accept = item->func(enter);
	if (!accept || !item->items)
		goto ret;
	if (enter) {
		stack[stackSize++] = currentItem();
		display(item);
	} else
		toUpperLevel();
	return;
ret:
	if (!tft::transform())
		refresh();
	else
		tft::setTransform(false);
}

void PortraitList::pool(rTouch *touch)
{
	rTouch::coord_t pos = touch->position();
	switch(touch->status()) {
	case rTouch::Pressed:
		pressed = true;
		break;
	case rTouch::Moved:
		pressed = false;
		if (prev.x > 0 && (int16_t)scroll() - pos.y + prev.y >= 0)
			setScroll((int16_t)scroll() - pos.y + prev.y);
		break;
	case rTouch::Idle:
		if (pressed && prev.x > 0)
			clickOn(prev.x, prev.y);
		pressed = false;
		break;
	}
#ifdef DEBUG
	using namespace colours::b16;
	tft::rectangle(0, tft::topEdge() - 1, tft::width(), 1, White);
	tft::rectangle(0, tft::bottomEdge(), tft::width(), 1, White);
	tft::rectangle(0, tft::bottomEdge() - 1, tft::width(), 1, Black);
#endif
	prev.x = pos.x;
	prev.y = pos.y;
}
