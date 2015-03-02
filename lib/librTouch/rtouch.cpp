#include <avr/io.h>
#include <avr/interrupt.h>
#include <adc.h>
#include <eemem.h>
#include <pcint.h>
#include "rtouch.h"
#include "ts_calibrate.h"

// Interface functions
// Ports	PORTA		PORTB
// Function	X+	Y+	X-	Y-
// Detection	DINPUT	High-Z	High-Z	VSS
// Read X	VCC	ADC	VSS	High-Z
// Read Y	ADC	VCC	High-Z	VSS

// Calibration cross size
#define CALIB_SIZE	10

#define RTOUCH_DETECT()	(!(RTOUCH_PINP & RTOUCH_XP))

enum Functions {Detection = 0, ReadX, ReadY};

int32_t EEMEM rTouch::NVcal[sizeof(rTouch::cal) / sizeof(rTouch::cal[0])];
static struct {
	volatile bool pressed;
	rTouch::coord_t pos, postmp;
} ts;
static struct averager_t {
	uint8_t level, current;
	uint16_t x[RTOUCH_AVERAGER], y[RTOUCH_AVERAGER];
} averager = {0, 0};

static inline void rTouchMode(Functions func);
static void rTouchADCISR(uint8_t channel, uint16_t result);

rTouch::rTouch(tft_t *tft)
{
	this->tft = tft;
	prevRead.x = 0;
	prevRead.y = 0;
	calibrated = false;
}

void rTouch::init(void)
{
	ts.pressed = false;
	rTouchMode(Detection);
	pcint_set(RTOUCH_PCMSK, RTOUCH_XP);
	pcint_enable(RTOUCH_PCMSK);
	adc_register_ISR(rTouchADCISR);
}

const rTouch::coord_t rTouch::coordTranslate(coord_t pos) const
{
	pos.x = (cal[0] * (int32_t)pos.x + \
		cal[1] * (int32_t)pos.y + cal[2]) / cal[6];
	pos.y = (cal[3] * (int32_t)pos.y + \
		cal[4] * (int32_t)pos.y + cal[5]) / cal[6];
#ifndef RTOUCH_SWAPXY
	if (tft->portrait()) {
#else
	if (!tft->portrait()) {
#endif
		int16_t tmp = pos.x;
#ifndef RTOUCH_SWAPXY
		pos.x = tft->width() - pos.y;
		pos.y = tmp;
#else
		pos.x = pos.y;
		pos.y = tft->height() - tmp;
#endif
	}
	if (tft->flipped()) {
		pos.x = (int16_t)tft->width() - pos.x;
		pos.y = (int16_t)tft->height() - pos.y;
	}
	return pos;
}

const rTouch::coord_t rTouch::position(void)
{
	uint32_t x = 0, y = 0;
	for (uint8_t i = 0; i < RTOUCH_AVERAGER; i++) {
		x += averager.x[i];
		y += averager.y[i];
	}
	coord_t res = {(int16_t)(x / RTOUCH_AVERAGER), (int16_t)(y / RTOUCH_AVERAGER)};
	return calibrated ? coordTranslate(res) : res;
}

void rTouch::drawCross(const coord_t pos, uint16_t c)
{
	tft->rectangle(pos.x - CALIB_SIZE, pos.y, CALIB_SIZE * 2, 1, c);
	tft->rectangle(pos.x, pos.y - CALIB_SIZE, 1, CALIB_SIZE * 2, c);
}

const rTouch::coord_t rTouch::calibrationPoint(const uint8_t index)
{
	coord_t pos;
	if (index == 0 || index == 2)
		pos.x = tft->width() * 0.2;
	else if (index == 1 || index == 3)
		pos.x = tft->width() * 0.8;
	else
		pos.x = tft->width() / 2;
	if (index == 0 || index == 1)
		pos.y = tft->height() * 0.2;
	else if (index == 2 || index == 3)
		pos.y = tft->height() * 0.8;
	else
		pos.y = tft->height() / 2;
	return pos;
}

bool rTouch::pressed(void)
{
	return ts.pressed;
}

const rTouch::coord_t rTouch::waitForPress(void)
{
	while (!pressed());
	while (pressed());
	return position();
}

void rTouch::calibrate(void)
{
	if (!eeprom_first()) {
		eeprom_read_block(cal, NVcal, sizeof(NVcal));
		calibrated = true;
		return;
	}
	tft->setBackground(0x0000);
	tft->setForeground(0x667F);
	tft->clean();
	tft->setZoom(2);
	uint8_t orient = tft->orient();
	tft->setXY((tft->width() - FONT_WIDTH * tft->zoom() * 11) / 2, \
		   (tft->height() - FONT_HEIGHT * tft->zoom() ) / 3);
	(*tft) << "Calibration";
#ifndef RTOUCH_SWAPXY
	tft->setOrient(tft_t::Landscape);
#else
	tft->setOrient(tft_t::Portrait);
#endif

	calibration caldata;
	calibrated = false;
recalibrate:
	for (uint8_t i = 0; i < 5; i++) {
		const coord_t pos = calibrationPoint(i);
		drawCross(pos, 0xFFFF);
		const coord_t ts = waitForPress();
		caldata.x[i] = ts.x;
		caldata.xfb[i] = pos.x;
		caldata.y[i] = ts.y;
		caldata.yfb[i] = pos.y;
		drawCross(pos, 0x0000);
	}

	if (!perform_calibration(&caldata))
		goto recalibrate;

	tft->setOrient(orient);

	cal[0] = caldata.a[1];	// xscale
	cal[1] = caldata.a[2];	// xymix
	cal[2] = caldata.a[0];	// xoffset
	cal[3] = caldata.a[4];	// yxmix
	cal[4] = caldata.a[5];	// yscale
	cal[5] = caldata.a[3];	// yoffset
	cal[6] = caldata.a[6];	// scale
	calibrated = true;

	eeprom_update_block(cal, NVcal, sizeof(NVcal));
}

// For FAST operation, Detection -> ReadY -> ReadX -> Detection ONLY!
static inline void rTouchMode(Functions func)
{
	switch (func) {
	case Detection:
		RTOUCH_DDRM &= ~RTOUCH_XM;
		RTOUCH_DDRM |= RTOUCH_YM;
		RTOUCH_PORTM &= ~(RTOUCH_XM | RTOUCH_YM);
		RTOUCH_DDRP &= ~(RTOUCH_XP | RTOUCH_YP);
		RTOUCH_PORTP |= RTOUCH_XP;
		RTOUCH_PORTP &= ~RTOUCH_YP;
		DIDR0 &= ~RTOUCH_XP;
		DIDR0 |= RTOUCH_YP;
		break;
	case ReadY:
#ifdef RTOUCH_SAFE
		RTOUCH_DDRM &= ~RTOUCH_XM;
		RTOUCH_DDRM |= RTOUCH_YM;
		RTOUCH_PORTM &= ~(RTOUCH_XM | RTOUCH_YM);
		RTOUCH_DDRP &= ~RTOUCH_XP;
#endif
		RTOUCH_DDRP |= RTOUCH_YP;
		RTOUCH_PORTP &= ~RTOUCH_XP;
		RTOUCH_PORTP |= RTOUCH_YP;
		DIDR0 &= ~RTOUCH_YP;
		DIDR0 |= RTOUCH_XP;
		break;
	case ReadX:
		RTOUCH_DDRM |= RTOUCH_XM;
		RTOUCH_DDRM &= ~RTOUCH_YM;
#ifdef RTOUCH_SAFE
		RTOUCH_PORTM &= ~(RTOUCH_XM | RTOUCH_YM);
#endif
		RTOUCH_DDRP |= RTOUCH_XP;
		RTOUCH_DDRP &= ~RTOUCH_YP;
		RTOUCH_PORTP |= RTOUCH_XP;
		RTOUCH_PORTP &= ~RTOUCH_YP;
		DIDR0 &= ~RTOUCH_XP;
		DIDR0 |= RTOUCH_YP;
		break;
	};
}

static inline bool rTouchAverager(uint16_t x, uint16_t y)
{
	if (!averager.level || abs(x - averager.x[averager.current]) + abs(y - averager.y[averager.current]) > RTOUCH_DELTA) {
		averager.level = 1;
		averager.current = 0;
		averager.x[0] = x;
		averager.y[0] = y;
		return false;
	}
	if (averager.level < RTOUCH_AVERAGER) {
		averager.x[averager.level] = x;
		averager.y[averager.level] = y;
		averager.current = averager.level++;
		return false;
	}
	if (++averager.current == RTOUCH_AVERAGER)
		averager.current = 0;
	averager.x[averager.current] = x;
	averager.y[averager.current] = y;
	return true;
}

// Detection -> ReadY -> ReadX -> Detection
static void rTouchADCISR(uint8_t channel, uint16_t result)
{
	PINB |= _BV(7);
	if (channel == RTOUCH_YC) {
		ts.postmp.y = result;
		rTouchMode(ReadX);
		adc_request(RTOUCH_XC);
	} else if (channel == RTOUCH_XC) {
		ts.postmp.x = result;
		rTouchMode(Detection);
		if (RTOUCH_DETECT()) {
			if (rTouchAverager(ts.postmp.x, ts.postmp.y)) {
				ts.pressed = true;
			}
			rTouchMode(ReadY);
			adc_request(RTOUCH_YC);
		} else {
			averager.level = 0;
			ts.pressed = false;
			pcint_enable(RTOUCH_PCMSK);
		}
	}
}

ISR(RTOUCH_PCMSKV)
{
	if (RTOUCH_DETECT()) {
		pcint_disable(RTOUCH_PCMSK);
		rTouchMode(ReadY);
		adc_request(RTOUCH_YC);
	}
}
