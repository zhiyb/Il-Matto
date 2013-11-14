#ifndef MMC_H
#define MMC_H

#define SD_DDR DDRB
#define SD_PORT PORTB
#define SD_PIN PINB

#define SD_CD (1 << 1)
#define SD_WP (1 << 0)

#include "spi.h"

class sdhw: public spi
{
public:
	sdhw(void);
	virtual uint8_t init(void);
	virtual bool detect(void);
	virtual bool writeProtected(void);

	uint8_t res[5], ver;

private:
	virtual void send(uint8_t index, uint32_t arg, uint8_t crc);
	virtual uint8_t recv(void);
};

extern class sdhw sd;

#endif
