#include <stdio.h>
#include "tft.h"

struct tfthw tft;

static inline int tftputch(const char c, FILE *stream)
{
	tft_print_char(c);
	return 0;
}

FILE *tftout(void)
{
	static FILE *out = NULL;
	if (out == NULL)
		out = fdevopen(tftputch, NULL);
	return out;
}
