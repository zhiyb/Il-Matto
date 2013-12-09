#ifndef ROTARY_H
#define ROTARY_H

#define ROE_DDR		DDRD
#define ROE_PORT	PORTD
#define ROE_PIN		PIND

#define ROE_A1	(1 << 2)
#define ROE_B1	(1 << 3)
#define ROE_S1	(1 << 4)

#define ROE_A2	(1 << 6)
#define ROE_B2	(1 << 7)
#define ROE_S2	(1 << 5)

#define ROE_N		0

#define ROE_CW1		1
#define ROE_CCW1	2
#define ROE_SW1		3

#define ROE_CW2		4
#define ROE_CCW2	5
#define ROE_SW2		6

void ROE_init(void);
uint8_t ROE_get(void);

#endif
