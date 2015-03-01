#ifndef MPU6050_H
#define MPU6050_H

#include <inttypes.h>

#define MPU6050_I2C_AD0		0
#define MPU6050_I2C_ADDR	((0b1101000 | MPU6050_I2C_AD0) << 1)
#define MPU6050_I2C_ADDR_WRITE	(MPU6050_I2C_ADDR | 0)
#define MPU6050_I2C_ADDR_READ	(MPU6050_I2C_ADDR | 1)

struct data3d_t {
	uint16_t x, y, z;
};

namespace MPU6050
{
	void init(void);
	bool ack(void);
	bool write(uint8_t addr, uint8_t count, uint8_t *data);
	inline bool write(uint8_t addr, uint8_t data) {return write(addr, 1, &data);}
	bool read(uint8_t addr, uint8_t count, uint8_t *data);
	uint8_t read(uint8_t addr);
	data3d_t readGyro(void);
	data3d_t readAccel(void);
	uint16_t readTemp(void);
}

//#include "ov7670regs.h"

/*struct regval_list {
	unsigned char reg_num;
	unsigned char value;
};

namespace OV7670
{
	void init(void);
	uint8_t read(const uint8_t addr);
	void write(const uint8_t addr, const uint8_t data);
	void writeArray(struct regval_list *vals);
	void writeArray(uint8_t *vals);
	void reset(void);
	void capture(void);
	void startCapture(void);
	void enableFIFO(bool e);
	uint8_t readFIFO(void);
}*/

#endif
