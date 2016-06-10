#include <stdint.h>
#include "colours.h"

uint32_t colour_hsv_to_rgb(uint32_t hsv)
{
	uint8_t h = H_888(hsv);
	uint8_t s = S_888(hsv);
	uint8_t v = V_888(hsv);

	// http://web.mit.edu/storborg/Public/hsvtorgb.c

	if (s == 0)
		return COLOUR_888(v, v, v);	// Colour is grayscale

	uint8_t region = h / 43;
	uint8_t fpart = (h - (region * 43)) * 6;
	
	uint8_t p = ((uint16_t)v * (255 - s)) >> 8;
	uint8_t q = ((uint16_t)v * (255 - (((uint16_t)s * fpart) >> 8))) >> 8;
	uint8_t t = ((uint16_t)v * (255 - (((uint16_t)s * (255 - fpart)) >> 8))) >> 8;

	uint8_t r, g, b;
	switch (region) {
		case 0:
			r = v; g = t; b = p; break;
		case 1:
			r = q; g = v; b = p; break;
		case 2:
			r = p; g = v; b = t; break;
		case 3:
			r = p; g = q; b = v; break;
		case 4:
			r = t; g = p; b = v; break;
		default:
			r = v; g = p; b = q; break;
	}

	return COLOUR_888(r, g, b);
}
