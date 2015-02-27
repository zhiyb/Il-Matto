#ifndef RESTOUCH_H
#define RESTOUCH_H

#include <tft.h>

#define RESTOUCH_DDRP	DDRA
#define RESTOUCH_PINP	PINA
#define RESTOUCH_PORTP	PORTA
#define RESTOUCH_DDRM	DDRB
#define RESTOUCH_PINM	PINB
#define RESTOUCH_PORTM	PORTB
#define RESTOUCH_XM	(1 << 5)
#define RESTOUCH_XP	(1 << 6)
#define RESTOUCH_XC	7
#define RESTOUCH_YM	(1 << 4)
#define RESTOUCH_YP	(1 << 7)
#define RESTOUCH_YC	6

class ResTouch
{
public:
	enum Functions {Detection, ReadX, ReadY};

	ResTouch(tft_t *tft);
	static void init(void);
	void calibration(void);
	static bool detect(void) {return function(Detection);}

	struct result_t {
		int16_t x, y;
	};

	static result_t read(void);

private:
	static void mode(const Functions func);
	static uint16_t function(const Functions func);

	struct {
		result_t data[4];
	} cal;

	tft_t *tft;
};

#endif
