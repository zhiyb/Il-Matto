#ifndef MPU6050_H
#define MPU6050_H

#include <inttypes.h>

#define MPU6050_I2C_AD0		0
#define MPU6050_I2C_ADDR	((0b1101000 | MPU6050_I2C_AD0) << 1)
#define MPU6050_I2C_ADDR_WRITE	(MPU6050_I2C_ADDR | 0)
#define MPU6050_I2C_ADDR_READ	(MPU6050_I2C_ADDR | 1)

struct data3d_t
{
	int16_t x, y, z;
};

namespace MPU6050
{
	bool init(void);
	bool ack(void);
	bool write(uint8_t addr, uint8_t count, uint8_t *data);
	inline bool write(uint8_t addr, uint8_t data) {return write(addr, 1, &data);}
	bool read(uint8_t addr, uint8_t count, uint8_t *data);
	uint8_t read(uint8_t addr);
	data3d_t readGyro(void);
	data3d_t readAccel(void);
	uint16_t readTemp(void);
}

#endif
