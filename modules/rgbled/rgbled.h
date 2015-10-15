// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef _RGBLED_H
#define _RGBLED_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RGBLED_PORT
#define RGBLED_PORT	D
#endif
#ifndef RGBLED_DIN
#define	RGBLED_DIN	7
#endif

#ifndef RGBLED_NUM
#define RGBLED_NUM	8
#endif

extern uint32_t rgbLED[RGBLED_NUM];

void rgbLED_init();
void rgbLED_refresh();

#ifdef __cplusplus
}
#endif

#endif
