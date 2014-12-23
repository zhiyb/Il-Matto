#include "ov7670.h"
#include "i2c.h"

#define IP(R)	(R ? 0x43 : 0x42)

void OV7670hw::init(void)
{
	i2c::init();
	OV_CTRLD &= ~(OV_VSYNC | OV_HREF);
	OV_CTRLD |= OV_WEN | OV_RRST | OV_OE | OV_RCLK;
	OV_CTRLW &= ~(OV_RRST);
	OV_CTRLW |= OV_VSYNC | OV_HREF | OV_WEN | OV_OE | OV_RCLK;
	OV_CTRLW &= ~OV_RCLK;	// OE is fetched at the rising edge of RCLK
	OV_CTRLW |= OV_RCLK;
}

uint8_t OV7670hw::read(const uint8_t addr)
{
	i2c::start();
	i2c::write(IP(0));
	i2c::write(addr);
	i2c::stop();
	i2c::start();
	i2c::write(IP(1));
	uint8_t data = i2c::read();
	i2c::stop();
	return data;
}

void OV7670hw::write(const uint8_t addr, const uint8_t data)
{
	i2c::start();
	i2c::write(IP(0));
	i2c::write(addr);
	i2c::stop();
	i2c::start();
	i2c::write(IP(0));
	i2c::write(data);
	i2c::stop();
}
