#ifndef RESTOUCH_H
#define RESTOUCH_H

#include <avr/eeprom.h>
#include <tft.h>

#define RESTOUCH_DDRP	DDRA
#define RESTOUCH_PINP	PINA
#define RESTOUCH_PORTP	PORTA
#define RESTOUCH_DDRM	DDRB
#define RESTOUCH_PINM	PINB
#define RESTOUCH_PORTM	PORTB
#define RESTOUCH_XM	(1 << 5)
#define RESTOUCH_YM	(1 << 4)
#define RESTOUCH_XP	(1 << 6)
#define RESTOUCH_YP	(1 << 7)
#define RESTOUCH_XC	7
#define RESTOUCH_YC	6

#define RESTOUCH_SWAPXY
#define RESTOUCH_DELTA	5

class ResTouch
{
public:
	enum Functions {Detection, ReadX, ReadY};

	struct coord_t {
		int16_t x, y;
	};

	ResTouch(tft_t *tft);
	static void init(void);
	static bool detect(void) {return function(Detection);}
	const coord_t read(void);
	const coord_t waitForPress(void);
	void calibrate(void);

private:
	static void mode(const Functions func);
	static uint16_t function(const Functions func);
	void drawCross(const coord_t pos, uint16_t c);
	const coord_t calibrationPoint(const uint8_t index);
	const coord_t coordTranslate(coord_t pos) const;

	bool calibrated;
	int32_t cal[7];
	static int32_t EEMEM NVcal[sizeof(ResTouch::cal) / sizeof(ResTouch::cal[0])];
	coord_t prevRead;

	tft_t *tft;
};

#endif
