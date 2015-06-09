/*
 *  tslib/tests/ts_calibrate.c
 *  https://github.com/kergoth/tslib/blob/master/tests/ts_calibrate.c
 *
 *  Copyright (C) 2001 Russell King.
 *
 * This file is placed under the GPL.  Please see the file
 * COPYING for more details.
 *
 * $Id: ts_calibrate.c,v 1.8 2004/10/19 22:01:27 dlowder Exp $
 * Modified by Yubo Zhi for D4 project
 *
 * Basic test program for touchscreen library.
 */
#ifndef TS_CALIBRATE_H
#define TS_CALIBRATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

typedef struct {
	uint16_t x[5], xfb[5];
	uint16_t y[5], yfb[5];
	// Pointercal: xscale, xymix, xoffset, yxmix, yscale, yoffset, divider
	// xoffset, xscale, xymix, yoffset, yxmix, yscale, divider
	int32_t a[7];
} calibration;

uint8_t perform_calibration(calibration *cal);

#ifdef __cplusplus
}
#endif

#endif
