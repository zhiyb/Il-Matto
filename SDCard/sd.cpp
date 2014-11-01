#include "sd.h"
#include <util/delay.h>

sdhw_t::sdhw_t(void) : hw_t()
{
	SD_DDR &= ~(SD_CD | SD_WP);
	SD_PORT |= SD_CD | SD_WP;
	spi::init();
	spi::slow();
	spi::free(true);
	errno = 0;
}

bool sdhw_t::detect(void)
{
	if (SD_PIN & SD_CD)
		return false;
	_delay_ms(500);
	bool wp;
loop:
        wp = writeProtected();
        _delay_ms(200);
        if (writeProtected() != wp)
                goto loop;
        return true;
}

uint8_t sdhw_t::init(void)
{
	if (SD_PIN & SD_CD)
		return 1;
	spi::free(false);
	spi::slow();
	_delay_ms(1);				// Wait >= 1ms
	spi::assert(false);
	for (uint8_t i = 0; i < 10; i++)
		spi::trans();			// >= 74 dummy clock
	if ((errno = cmd(GO_IDLE_STATE, 0, GO_IDLE_STATE_CRC)) > 0x01)
		return free(2);
	if ((errno = cmd(SEND_IF_COND, SEND_IF_COND_ARG, SEND_IF_COND_CRC)) > 0x01)
		return free(3);
	if ((spi::trans32big() & ~(0xFFFFFFFF << 12)) != SEND_IF_COND_ARG)
		return free(4);
init:
	switch (errno = acmd(APP_SEND_OP_COND, 0x40000000)) {
	case 0x00:
		goto initFin;
	case 0x01:
		goto init;
	default:
		return free(5);
	}
initFin:
	if ((errno = cmd(READ_OCR)) != 0x00)
		return free(6);
	if (!(spi::trans32big() & 0x40000000))
		return free(7);
	spi::fast();
	_size = getSize();
	return free(0);
}

uint32_t sdhw_t::getSize(void)
{
	struct reg_t csd = readRegister(SEND_CSD);
	if (errno)
		return 0;
	else
		return (((uint32_t)csd.data[7] << 16) | ((uint32_t)csd.data[8] << 8) | csd.data[9]) * 512;
}

struct reg_t sdhw_t::readRegister(const uint8_t type)
{
	struct reg_t r;
	spi::free(false);
	if ((errno = cmd(type)) != 0x00)
		goto ret;
	if ((errno = response()) != 0xFE)
		goto ret;
	for (uint8_t i = 0; i < 16; i++)
		r.data[i] = spi::trans();
	readCRC();
	errno = 0;
ret:
	free();
	return r;
}
