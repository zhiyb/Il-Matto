// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef _RGBLED_H
#define _RGBLED_H

#define RGBLED_PORT	D
#define	RGBLED_DIN	0

#define RGBLED_NUM	8

extern uint32_t rgbLED[RGBLED_NUM];

void rgbLED_init();
void rgbLED_refresh();

#endif
