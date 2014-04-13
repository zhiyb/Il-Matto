#ifndef MBR_H
#define MBR_H

class mbr_t
{
public:
	inline void setEntry(const uint8_t index, uint8_t data[]);

	uint8_t type[4];
	uint32_t addr[4];
};

inline void mbr_t::setEntry(const uint8_t index, uint8_t data[])
{
	type[index] = data[4];
	for (uint8_t i = 0; i < 4; i++) {
	        addr[index] <<= 8;
		addr[index] |= data[8 + (3 - i)];
	}
}

#endif
