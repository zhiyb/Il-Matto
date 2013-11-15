#include "fat32.h"

void fat32::init(const class partition& p)
{
	_status = OK;
	switch (p.type()) {
	case 0x0B:
	case 0x0C:
		break;
	default:
		_status = TYPE_ERROR;
		return;
	}
}
