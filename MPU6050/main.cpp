#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <tft.h>
#include <mpu6050.h>

tft_t tft;

void init(void)
{
	MPU6050::init();
	tft.init();
	tft.setOrient(tft.Portrait);
	tft.setForeground(0x667F);
	tft.setBackground(0x0000);
	tft.clean();
	stdout = tftout(&tft);
	tft.setBGLight(true);
}

int main(void)
{
	//uint8_t cnt = 0;
	init();

	tft.clean();
	tft.setZoom(1);

start:
	tft.setXY(0, 0);
	puts("*** MPU6050 ***");
	printf("ACK: %u\n", MPU6050::ack());
#if 0
	for (uint8_t i = 0x0D; i <= 0x75; i++)
		printf("%02X/%02X\t", i, MPU6050::read(i));
	printf("\nCNT: %02X\n", cnt++);
#endif
	data3d_t gyro = MPU6050::readGyro();
	printf("GYRO:\n%d\n%d\n%d\n", gyro.x, gyro.y, gyro.z);
	data3d_t accel = MPU6050::readAccel();
	printf("ACCEL:\n%d\n%d\n%d\n", accel.x, accel.y, accel.z);
	printf("TEMP:  0x%04X\n", MPU6050::readTemp());
	goto start;

	return 1;
}
