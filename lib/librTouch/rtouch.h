#ifndef RTOUCH_H
#define RTOUCH_H

#include <avr/eeprom.h>
#include <avr/io.h>
#include <tft.h>

//#define RTOUCH_SAFE

// X+ and Y+
#define RTOUCH_DDRP	DDRA
#define RTOUCH_PINP	PINA
#define RTOUCH_PORTP	PORTA
// Pin change interrupt mask group
#define RTOUCH_PCMSK	0
// Pin change interrupt vector
#define RTOUCH_PCMSKV	PCINT0_vect

// X- and Y-
#define RTOUCH_DDRM	DDRB
#define RTOUCH_PINM	PINB
#define RTOUCH_PORTM	PORTB

// X+ and X-
#define RTOUCH_XM	(1 << 5)
#define RTOUCH_XP	(1 << 6)
#define RTOUCH_XC	7

// Y+ and Y-
#define RTOUCH_YM	(1 << 4)
#define RTOUCH_YP	(1 << 7)
#define RTOUCH_YC	6

#define RTOUCH_DELTA	5

class rTouch
{
	friend void rTouchADCISR(uint8_t channel, uint16_t result);
	friend void rTouchPCIISR(void);
public:
	enum Functions {Detection, ReadX, ReadY};

	struct coord_t {
		int16_t x, y;
	};

	rTouch(tft_t *tft);
	void init(void);
	const coord_t position(void);
	const coord_t waitForPress(void);
	void calibrate(void);
	static bool pressed(void);

private:
	static void mode(const Functions func);
	void drawCross(const coord_t pos, uint16_t c);
	const coord_t calibrationPoint(const uint8_t index);
	const coord_t coordTranslate(coord_t pos) const;

	bool calibrated;
	int32_t cal[7];
	static int32_t EEMEM NVcal[sizeof(rTouch::cal) / sizeof(rTouch::cal[0])];
	coord_t prevRead;

	tft_t *tft;
};

#endif
