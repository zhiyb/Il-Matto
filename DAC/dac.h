#ifndef DAC_H
#define DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#define CTRL_DAC_ID		10
#define CTRL_DAC_CHANNELS	4
#define CTRL_DAC_VALUE_BYTES	CTRL_BYTE1
#define CTRL_DAC_VALUE_TYPE	CTRL_DAC_VALUE_BYTES
#define CTRL_DAC_VALUE_MIN	0x00
#define CTRL_DAC_VALUE_MAX	0xFF

#include <inttypes.h>

// DAC chip TLV5620 uses PORTD with UART1
// DAC_LDAC set to LOW by hardware
#ifdef DAC_USE_PORTD
#define DAC_LOAD	_BV(2)
#define DAC_DATA	_BV(3)
#define DAC_CLK		_BV(4)
#else
#define DAC_LOAD	_BV(4)
#define DAC_DATA	_BV(5)
#define DAC_CLK		_BV(7)
#endif

void initDAC(void);
void setDAC(uint8_t ch, uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
