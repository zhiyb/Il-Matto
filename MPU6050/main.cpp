#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <mpu6050.h>
#include <tft.h>
#include <colours.h>

using namespace colours::b16;

void init(void)
{
	while (!MPU6050::init());
	tft::init();
	tft::setOrient(tft::Portrait);
	tft::foreground = 0x667f;
	tft::background = 0x0000;
	tft::clean();
	stdout = tft::devout();
	tft::setBGLight(true);
}

int main(void)
{
	init();

	tft::clean();
	tft::zoom = 1;

start:
	tft::x = 0;
	tft::y = 0;
	puts("*** MPU6050 ***");
	printf("ACK: %u\n", MPU6050::ack());
#if 0
	for (uint8_t i = 0x0D; i <= 0x75; i++)
		printf("%02X/%02X\t", i, MPU6050::read(i));
	printf("\nCNT: %02X\n", cnt++);
#endif
	data3d_t gyro = MPU6050::readGyro();
	printf("GYRO:\n%6d\n%6d\n%6d\n", gyro.x, gyro.y, gyro.z);
	data3d_t accel = MPU6050::readAccel();
	printf("ACCEL:\n%6d\n%6d\n%6d\n", accel.x, accel.y, accel.z);
	printf("TEMP:  0x%04X\n", MPU6050::readTemp());

	tft::setOrient(tft::Landscape);
disp:
	static const uint16_t clr[7] = {Red, Green, Blue, \
		Yellow, Cyan, Magenta, White};
	for (uint16_t x = 0; x < tft::width; x++) {
		int16_t data[7];
		data3d_t gyro = MPU6050::readGyro();
		data[0] = gyro.x;
		data[1] = gyro.y;
		data[2] = gyro.z;
		data3d_t accel = MPU6050::readAccel();
		data[3] = accel.x;
		data[4] = accel.y;
		data[5] = accel.z;
		data[6] = MPU6050::readTemp();
		tft::rectangle(x, 0, 1, tft::height, Black);
		for (uint8_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
			int32_t y = (int32_t)data[i] * tft::height \
				    / 0x00010000 + tft::height / 2;
			tft::point(x, y, clr[i]);
		}
	}
	goto disp;
	goto start;

	return 1;
}
