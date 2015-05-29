#ifndef MPU6050REGS_H
#define MPU6050REGS_H

#define SELF_TEST_X		0x0D
#define SELF_TEST_Y		0x0E
#define SELF_TEST_Z		0x0F
#define SELF_TEST_A		0x10

#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define  C_EXT_SYNC_SET		3	// [5:3]
#define  C_DLPF_CFG		0	// [2:0]
#define GYRO_CONFIG		0x1B
#define  GC_FS_SEL		3	// [4:3]
#define ACCEL_CONFIG		0x1C
#define  AC_XA_ST		0x80
#define  AC_YA_ST		0x40
#define  AC_ZA_ST		0x20
#define  AC_AFS_SEL		3	// [4:3]
#define FIFO_EN			0x23
#define  FE_TEMP_FIFO_EN	0x80
#define  FE_XG_FIFO_EN		0x40
#define  FE_YG_FIFO_EN		0x20
#define  FE_ZG_FIFO_EN		0x10
#define  FE_ACCEL_FIFO_EN	0x08
//#define  FE_SLV_EN(i)		(1 << i)	// 0, 1, 2
#define I2C_MST_CTRL		0x24
#define  IMC_MULT_MST_EN	0x80
#define  IMC_WAIT_FOR_ES	0x40
//#define  IMC_SLV_3_FIFO_EN	0x20
#define  IMC_I2C_MST_P_NSR	0x10
#define  IMC_I2C_MST_CLK	0	// [3:0]
//#define I2C_SLV0_ADDR		0x25
#define I2C_MST_STATUS		0x36
#define  IMS_PASS_THROUGH	0x80
//#define  IMS_I2C_SLV4_DONE	0x40
#define  IMS_I2C_LOST_ARB	0x20
//#define  IMS_I2C_SLV_NACK(i)	(1 << i)	// 0, 1, 2, 3, 4
#define INT_PIN_CFG		0x37
#define  IPC_INT_LEVEL		0x80
#define  IPC_INT_OPEN		0x40
#define  IPC_LATCH_INT_EN	0x20
#define  IPC_INT_RD_CLEAR	0x10
#define  IPC_FSYNC_INT_LEVEL	0x08
#define  IPC_FSYNC_INT_EN	0x04
#define  IPC_I2C_BYPASS_EN	0x02
#define INT_ENABLE		0x38
//
#define INT_STATUS		0x3A
//

#define  OUT_X			0
#define  OUT_Y			1
#define  OUT_Z			2
#define  OUT_XYZ(addr, xyz)	(addr + 2 * xyz)
#define  OUT_H			0
#define  OUT_L			1
#define  OUT_HL(addr, hl)	(addr + hl)
#define ACCEL_OUT		0x3B
//#define ACCEL_OUT(xyz, hl)	(0x3B + 2 * xyz + hl)
#define TEMP_OUT		0x41
//#define TEMP_OUT(hl)		(0x41 + hl)
#define GYRO_OUT		0x43
//#define GYRO_OUT(xyz, hl)	(0x43 + 2 * xyz + hl)
#define EXT_SENS_DATA(i)	(0x49 + i)	// i from 0 to 23

//#define I2C_SLV_DO(i)		(0x63 + i)	// 0, 1, 2, 3
#define I2C_MST_DELAY_CTRL	0x67
#define  IMDC_DELAY_ES_SHADOW	0x80
//#define  IMDC_I2C_SLV_DLY_EN(i)	(1 << i)	// 0, 1, 2, 3, 4
#define SIGNAL_PATH_RESET	0x68
#define  SPR_GYRO_RESET		0x04
#define  SPR_ACCEL_RESET	0x02
#define  SPR_TEMP_RESET		0x01
#define USER_CTRL		0x6A
#define  UC_FIFO_EN		0x40
#define  UC_I2C_MST_EN		0x20
#define  UC_I2C_IF_DIS		0x10
#define  UC_FIFO_RESET		0x04
#define  UC_I2C_MST_RESET	0x02
#define  UC_SIG_COND_RESET	0x01
#define PWR_MGMT_1		0x6B
#define  PM1_DEVICE_RESET	0x80
#define  PM1_SLEEP		0x40
#define  PM1_CYCLE		0x20
#define  PM1_TEMP_DIS		0x08
#define  PM1_CLKSEL		0	// [2:0]
#define  PM1_CLK_INTERNAL	0
#define  PM1_CLK_GYRO_X		1
#define  PM1_CLK_GYRO_Y		2
#define  PM1_CLK_GYRO_Z		3
#define  PM1_CLK_EXT_LOW	4
#define  PM1_CLK_EXT_HIGH	5
#define  PM1_CLK_STOP		7
#define PWR_MGMT_2
#define  PM2_LP_WAKE_CTRL	6	// [7:6]
#define  PM2_STBY_XA		0x20
#define  PM2_STBY_YA		0x10
#define  PM2_STBY_ZA		0x08
#define  PM2_STBY_XG		0x04
#define  PM2_STBY_YG		0x02
#define  PM2_STBY_ZG		0x01
#define FIFO_COUNT(hl)		(0x72 + hl)
#define FIFO_R_W		0x74
#define WHO_AM_I		0x75

#endif
