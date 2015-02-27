#ifndef LIST_H
#define LIST_H

struct listItem {
	const char *name;
	const listItem **items;
	listItem *parent;
	void (*func)(void);
};

#endif
