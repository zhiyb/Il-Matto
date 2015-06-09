#include <util/delay.h>
#include <i2c.h>
#include "mpu6050.h"
#include "mpu6050regs.h"

namespace MPU6050
{
	uint16_t readData(uint8_t addr);
	data3d_t readData3D(uint8_t aaddr);
}

bool MPU6050::init(void)
{
	i2c::init();
	_delay_ms(3);
	if (!ack())
		return false;
	if (!write(PWR_MGMT_1, PM1_DEVICE_RESET))
		return false;
	while (read(PWR_MGMT_1) & PM1_DEVICE_RESET);
	//_delay_ms(100);
	if (!write(USER_CTRL, UC_FIFO_RESET | UC_I2C_MST_RESET | UC_SIG_COND_RESET))
		return false;
	_delay_ms(100);
	return write(PWR_MGMT_1, PM1_CLK_GYRO_X);
}

bool MPU6050::ack(void)
{
	i2c::start();
	bool a = i2c::write(MPU6050_I2C_ADDR_WRITE);
	i2c::stop();
	return a;
}

bool MPU6050::write(uint8_t addr, uint8_t count, uint8_t *data)
{
	i2c::start();
	if (!i2c::write(MPU6050_I2C_ADDR_WRITE))
		return false;
	if (!i2c::write(addr))
		return false;
	while (count--)
		if (!i2c::write(*data++))
			return false;
	i2c::stop();
	return true;
}

bool MPU6050::read(uint8_t addr, uint8_t count, uint8_t *data)
{
	i2c::start();
	if (!i2c::write(MPU6050_I2C_ADDR_WRITE))
		return false;
	if (!i2c::write(addr))
		return false;
	i2c::start();
	if (!i2c::write(MPU6050_I2C_ADDR_READ))
		return false;
	while (count--)
		*data++ = i2c::read(count != 0);
	i2c::stop();
	return true;
}

uint8_t MPU6050::read(uint8_t addr)
{
	uint8_t data = 0;
	if (!read(addr, 1, &data))
		return 0;
	return data;
}

uint16_t MPU6050::readData(uint8_t addr)
{
	uint16_t v = 0;
	v = ((uint16_t)read(OUT_HL(addr, OUT_H))) << 8;
	v |= read(OUT_HL(addr, OUT_L));
	return v;
}

data3d_t MPU6050::readGyro(void)
{
	return readData3D(GYRO_OUT);
}

data3d_t MPU6050::readAccel(void)
{
	return readData3D(ACCEL_OUT);
}

data3d_t MPU6050::readData3D(uint8_t addr)
{
	data3d_t data;
	data.x = readData(OUT_XYZ(addr, OUT_X));
	data.y = readData(OUT_XYZ(addr, OUT_Y));
	data.z = readData(OUT_XYZ(addr, OUT_Z));
	return data;
}

uint16_t MPU6050::readTemp(void)
{
	return readData(TEMP_OUT);
}
