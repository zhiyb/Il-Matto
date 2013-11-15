#include "sd.h"
#include "disk.h"

partition::partition(uint8_t dat[16])
{
	_type = dat[4];
	for (uint8_t i = 0; i < 4; i++) {
		_begin <<= 8;
		_begin |= dat[8 + (3 - i)];
	}
}

void disk::init(void)
{
	_status = SUCCEED;
	if (sd.version() == 0) {
		_status = NOT_INIT;
		return;
	}
	uint8_t block[512];
	sd.readBlock(0, block);
	if (block[510] != 0x55 || block[511] != 0xAA) {
		_status = MBR_SIG_FAILED;
		return;
	}
	for (uint8_t i = 0; i < 3; i++)
		_part[i] = partition(&block[446 + 16 * i]);
}
