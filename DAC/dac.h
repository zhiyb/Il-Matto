#ifndef DAC_H
#define DAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>

#define DAC_CHANNEL	0

// DAC chip TLV5620 uses PORTD with UART1
// DAC_LDAC set to LOW by hardware
#define DAC_LOAD	(1 << 2)
#define DAC_DATA	(1 << 3)
#define DAC_CLK		(1 << 4)

void init_dac(void);
void set_dac(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
