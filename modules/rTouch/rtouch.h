/*
 * Author: Yubo Zhi (yz39g13@soton.ac.uk)
 */

#ifndef RTOUCH_H
#define RTOUCH_H

#include <avr/eeprom.h>
#include <avr/io.h>
#include <tft.h>
#include <adcrequest.h>

// Safe mode switches, generally unnecessary
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

// Data valid variance (maximum)
#ifndef RTOUCH_DELTA
#define RTOUCH_DELTA	24
#endif

// Averager depth
#ifndef RTOUCH_AVERAGER
#define RTOUCH_AVERAGER	16
#endif

// Press / move threshold
#ifndef RTOUCH_MOVETH
#define RTOUCH_MOVETH	16
#endif

class rTouch
{
public:
	enum Status {Idle = 0, Pressed, Moved};

	struct coord_t {
		int16_t x, y;
	};

	rTouch(adcRequest_t *req);
	void init(void);
	const coord_t position(void);
	const coord_t waitForPress(void);
	void calibrate(bool reset = false);
	void recalibrate(void) {calibrate(true);}
	static bool pressed(void);
	Status status(void);

private:
	void drawCross(const coord_t pos, uint16_t c);
	const coord_t calibrationPoint(const uint8_t index);
	const coord_t coordTranslate(coord_t pos) const;

	struct {
		bool pressed, moved;
		coord_t prev;
	} stat;

	bool calibrated;
	int32_t cal[7];
	static int32_t EEMEM NVcal[7];
};

#endif
