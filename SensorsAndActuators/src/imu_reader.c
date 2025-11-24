/*
 * imu_reader.c
 *
 *  Created on: Nov 19, 2025
 *      Author: marco
 */
#include "imu_reader.h"

#include "i2c_busses.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern I2C_HandleTypeDef hi2c1;
extern SemaphoreHandle_t i2c1_mutex;

#define I2C_HANDLE hi2c1
#define I2C_MUTEX i2c1_mutex


#define I2C_MPU6500_ADDRESS 0x68

#define MPU6500_WHOAMI 0x75

#define MPU6500_ACCEL_XOUT_H 0x3B
#define MPU6500_ACCEL_XOUT_L 0x3C
#define MPU6500_ACCEL_YOUT_H 0x3D
#define MPU6500_ACCEL_YOUT_L 0x3E
#define MPU6500_ACCEL_ZOUT_H 0x3F
#define MPU6500_ACCEL_ZOUT_L 0x40

#define MPU6500_TEMP_OUT_H 0x41
#define MPU6500_TEMP_OUT_L 0x42

#define MPU6500_GYRO_XOUT_H 0x43
#define MPU6500_GYRO_XOUT_L 0x44
#define MPU6500_GYRO_YOUT_H 0x45
#define MPU6500_GYRO_YOUT_L 0x46
#define MPU6500_GYRO_ZOUT_H 0x47
#define MPU6500_GYRO_ZOUT_L 0x48

#define FIFO_ENABLE 0x23
#define FIFO_ENABLE_VALUE 0x78 // all gyro + accelerometer

#define GYRO_SCALE 250
#define ACCELEROMETER_SCALE 2 * 9.80665

#define BLOCK_TIME (TickType_t)2

RobottoErrorCode verifyIMUCommunication()
{
	uint8_t whoami = 0;

	if(ROBOTTO_OK != ReadI2C(ROBOTTO_DEVICE_IMU, I2C_MPU6500_ADDRESS, MPU6500_WHOAMI, &whoami, 1))
	{
		return ROBOTTO_ERROR;
	}

	if(whoami != 0x70)
	{
		SEGGER_SYSVIEW_ErrorfTarget("ERROR - whoami %u\n", &whoami);
		return ROBOTTO_ERROR;
	}
	return ROBOTTO_OK;
}

float toFloat(uint8_t high, uint8_t low, float max_scale)
{
    int16_t tmp = (int16_t)((int16_t)high << 8 | (uint16_t)low);
    return ((float)tmp) / 32768.0f * max_scale;
}

RobottoErrorCode readIMUData(ImuData* out)
{
	uint8_t rawImu[14];

	if(ROBOTTO_OK != ReadI2C(ROBOTTO_DEVICE_IMU, I2C_MPU6500_ADDRESS, MPU6500_ACCEL_XOUT_H, rawImu, 14))
	{
		return ROBOTTO_ERROR;
	}

    out->acc_x = toFloat(rawImu[0], rawImu[1], ACCELEROMETER_SCALE);
    out->acc_y = toFloat(rawImu[2], rawImu[3], ACCELEROMETER_SCALE);
    out->acc_z = toFloat(rawImu[4], rawImu[5], ACCELEROMETER_SCALE);

    out->gyro_x = toFloat(rawImu[8], rawImu[9], GYRO_SCALE);
    out->gyro_y = toFloat(rawImu[10], rawImu[11], GYRO_SCALE);
    out->gyro_z = toFloat(rawImu[12], rawImu[13], GYRO_SCALE);
    return ROBOTTO_OK;
}

