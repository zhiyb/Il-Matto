#include <avr/io.h>
#include "adc.h"
#include "restouch.h"
#include "ts_calibrate.h"

// Interface functions
// Ports	PORTA		PORTB
// Function	X+	Y+	X-	Y-
// Detection	DINPUT	High-Z	High-Z	VSS
// Read X	VCC	ADC	VSS	High-Z
// Read Y	ADC	VCC	High-Z	VSS

// Calibration cross size
#define CALIB_SIZE	10

ResTouch::ResTouch(tft_t *tft)
{
	this->tft = tft;
	calibrated = false;
}

void ResTouch::init(void)
{
	mode(Detection);
}

// For FAST operation, Detection -> ReadY -> ReadX -> Detection ONLY!
void ResTouch::mode(Functions func)
{
	switch (func) {
	case Detection:
		ADCSRA &= ~_BV(ADEN);
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~(RESTOUCH_XP | RESTOUCH_YP);
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_XP;
		DIDR0 |= RESTOUCH_YP;
		_delay_us(10);
		break;
	case ReadY:
#ifndef FAST
		RESTOUCH_DDRM &= ~RESTOUCH_XM;
		RESTOUCH_DDRM |= RESTOUCH_YM;
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
		RESTOUCH_DDRP &= ~RESTOUCH_XP;
#endif
		RESTOUCH_DDRP |= RESTOUCH_YP;
		RESTOUCH_PORTP &= ~RESTOUCH_XP;
		RESTOUCH_PORTP |= RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_YP;
		DIDR0 |= RESTOUCH_XP;
		adc::init(RESTOUCH_YC);
		adc::start();
		break;
	case ReadX:
		RESTOUCH_DDRM |= RESTOUCH_XM;
		RESTOUCH_DDRM &= ~RESTOUCH_YM;
#ifndef FAST
		RESTOUCH_PORTM &= ~(RESTOUCH_XM | RESTOUCH_YM);
#endif
		RESTOUCH_DDRP |= RESTOUCH_XP;
		RESTOUCH_DDRP &= ~RESTOUCH_YP;
		RESTOUCH_PORTP |= RESTOUCH_XP;
		RESTOUCH_PORTP &= ~RESTOUCH_YP;
		DIDR0 &= ~RESTOUCH_XP;
		DIDR0 |= RESTOUCH_YP;
		adc::init(RESTOUCH_XC);
		adc::start();
		break;
	};
}

uint16_t ResTouch::function(Functions func)
{
	switch (func) {
	case Detection:
		return !(RESTOUCH_PINP & RESTOUCH_XP);
	case ReadX:
	case ReadY:
		return adc::read();
	};
	return 0;
}

const ResTouch::coord_t ResTouch::coordTranslate(coord_t pos) const
{
	pos.x = (cal.ax * (int32_t)pos.x + \
		cal.bx * (int32_t)pos.y + cal.dx) / cal.scale;
	pos.y = (cal.ay * (int32_t)pos.y + \
		cal.by * (int32_t)pos.y + cal.dy) / cal.scale;
	if (tft->flipped()) {
		pos.x = (int16_t)tft->width() - pos.x;
		pos.y = (int16_t)tft->height() - pos.y;
	}
	return pos;
}

const ResTouch::coord_t ResTouch::read(void) const
{
	coord_t res;
	mode(ReadY);
	res.y = function(ReadY);
	mode(ReadX);
	res.x = function(ReadX);
	mode(Detection);
	if (calibrated)
		return coordTranslate(res);
	return res;
}

void ResTouch::drawCross(const coord_t pos, uint16_t c)
{
	tft->rectangle(pos.x - CALIB_SIZE, pos.y, CALIB_SIZE * 2, 1, c);
	tft->rectangle(pos.x, pos.y - CALIB_SIZE, 1, CALIB_SIZE * 2, c);
}

const ResTouch::coord_t ResTouch::calibrationPoint(const uint8_t index)
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

const ResTouch::coord_t ResTouch::waitForPress(void) const
{
	coord_t pos, posnew;
	while (!detect());
	while (detect()) {
		pos = posnew;
		posnew = read();
	}
	return pos;
}

void ResTouch::calibrate(void)
{
	tft->setBackground(0x0000);
	tft->setForeground(0x667F);
	tft->clean();
	tft->setZoom(2);
	uint8_t orient = tft->orient();
	tft->setXY((tft->width() - FONT_WIDTH * tft->zoom() * 11) / 2, \
		   (tft->height() - FONT_HEIGHT * tft->zoom() ) / 3);
	(*tft) << "Calibration";
	tft->setOrient(tft_t::Landscape);

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

	// xoffset, xscale, xymix, yoffset, yxmix, yscale, divider
	cal.ax = caldata.a[1];
	cal.bx = caldata.a[2];
	cal.dx = caldata.a[0];
	cal.ay = caldata.a[4];
	cal.by = caldata.a[5];
	cal.dy = caldata.a[3];
	cal.scale = caldata.a[6];
	calibrated = true;
}
