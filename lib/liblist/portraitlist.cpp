#include "portraitlist.h"

#define ZOOM		2
#define TOP_AREA	(tft->topFixedArea())
#define BOTTOM_AREA	(tft->bottomFixedArea())
#define MAX_AREA	(tft->vsMaximum())
#define SCROLL_AREA	(tft->vsHeight())

#define ITEM_NAME_X	16
#define ITEM_NAME_Y	4
#define ITEM_HEIGHT	(FONT_HEIGHT * ZOOM + ITEM_NAME_Y * 2)

#define DEF_TOP_AREA	(ITEM_HEIGHT * 2)
#define DEF_BOTTOM_AREA	(ITEM_HEIGHT * 1)

void PortraitList::refresh(void)
{
	tft->clean();
	tft->setZoom(ZOOM);
	display();
}

void PortraitList::display(listItem *item)
{
	tft->setVerticalScrollingArea(DEF_TOP_AREA, DEF_BOTTOM_AREA);
	if (!item) {
		item = currentItem();
		if (!item)
			return;
	} else {
		setCurrentItem(item);
		max = countItems() * ITEM_HEIGHT;
		if (max > SCROLL_AREA)
			max -= SCROLL_AREA;
		else
			max = 0;
		scr = 0;
	}
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	tft->setTopMask(0);
	tft->setBottomMask(0);
	tft->setY(0);
	displayItem(currentItem());
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

void PortraitList::displayItem(const listItem *item, const uint16_t index) const
{
	uint16_t ys = tft->y();
	if (index != (uint16_t)-1) {
		uint16_t y = index * ITEM_HEIGHT;		// Relative to TOP_AREA
		if (y + ITEM_HEIGHT < scroll())			// Out of scroll area
			return;
		if (y < scroll()) {				// Above TOP_AREA
			ys = TOP_AREA - (scroll() - y);
			tft->setY(tft->vsTransform(ys + ITEM_NAME_Y));
			goto disp;
		}
		y -= scroll();					// Relative to upperEdge
		if (y >= tft->vsHeight())			// Out of scroll area
			return;
		ys = tft->vsTransform(y + tft->topEdge());
		tft->setY(tft->vsTransform(y + tft->topEdge() + ITEM_NAME_Y));
	} else
		tft->setY(tft->vsTransform(tft->y()) + ITEM_NAME_Y);

	static uint16_t c[] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0x07FF, 0xF81F};
disp:
	tft->setBackground(c[index % (sizeof(c) / sizeof(c[1]))]);
	tft->rectangle(0, ys, tft->width(), ITEM_HEIGHT, tft->background());
	tft->setX(ITEM_NAME_X);
	*tft << (item ? item->name : "** EMPTY **");
}

void PortraitList::displayItems(const listItem **items, uint16_t index, uint16_t last) const
{
	tft->setTransform(true);
	uint16_t first = scroll() / ITEM_HEIGHT;
	if (index < first) {
		for (uint16_t i = index; i < first && *items != 0; i++)
			items++;
		index = first;
	}
	if (!last)
		last = (scroll() + SCROLL_AREA + ITEM_HEIGHT - 1) / ITEM_HEIGHT;
	while (index < last) {
		displayItem(*items, index++);
		if (*items != 0)
			items++;
	}
	tft->setTransform(false);
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

void PortraitList::setScroll(uint16_t s)
{
	if (s == scroll())
		return;
	else if (s > scroll())
		scrollDown(s - scroll());
	else
		scrollUp(scroll() - s);
}

void PortraitList::scrollDown(uint16_t s)
{
	uint16_t index = (scroll() + SCROLL_AREA) / ITEM_HEIGHT;
	uint16_t scrbak = scroll();
	scrollTo(scroll() + s);
	if ((s = scroll() - scrbak) == 0)
		return;
	tft->setTopMask(s > tft->vsHeight() ? tft->topEdge() : tft->bottomEdge() - s);
	tft->setBottomMask(tft->vsMaximum() - tft->bottomEdge());
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	const listItem **items = curItem->items;
	for (uint16_t i = 0; i < index && *items != 0; i++)
		items++;
	displayItems(items, index);
}

void PortraitList::scrollUp(uint16_t s)
{
	uint16_t last = (scroll() + ITEM_HEIGHT - 1) / ITEM_HEIGHT;
	uint16_t scrbak = scroll();
	scrollTo(scroll() - s);
	if ((s = scrbak - scroll()) == 0)
		return;
	tft->setTopMask(tft->topEdge());
	tft->setBottomMask(tft->vsMaximum() - (s > tft->vsHeight() ? tft->bottomEdge() : tft->topEdge() + s));
	tft->setVerticalScrolling(TOP_AREA + scroll() % SCROLL_AREA);
	uint16_t first = scroll() / ITEM_HEIGHT;
	const listItem **items = curItem->items;
	for (uint16_t i = 0; i < first && *items != 0; i++)
		items++;
	displayItems(items, first, last);
}
