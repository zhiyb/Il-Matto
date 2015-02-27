#ifndef RESTOUCH_H
#define RESTOUCH_H

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

namespace ResTouch {
	enum Functions {Detection, ReadX, ReadY};

	void init(void);
	void mode(Functions func);
	uint16_t function(Functions func);
}

#endif
