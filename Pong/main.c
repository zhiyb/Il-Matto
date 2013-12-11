#include <avr/interrupt.h>
#include "libili9341/tft.h"
#include "rotary.h"
#include "timer.h"
#include "bar.h"
#include "box.h"
#include "connect.h"
#include "sound.h"

static inline void init(void)
{
	tft_init();
	//tft_setOrient(Portrait);
	tft_setOrient(FlipLandscape);
	tft_clean();
	ROE_init();
	timer0_init();
	connect_init();
	sound_init();
	sei();
	tft_setBGLight(1);
}

int __attribute__((noreturn)) main(void)
{
	init();

	while (1)
		game_main();
}
