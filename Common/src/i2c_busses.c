/*
 * i2c_busses.c
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */

#include "i2c_busses.h"

#include "FreeRTOS.h"
#include "semphr.h"


#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

#define MUTEX_BLOCK_TICKS (TickType_t)2
#define I2C_TIMEOUT 100

extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;

SemaphoreHandle_t i2c1_mutex = NULL;
SemaphoreHandle_t i2c2_mutex = NULL;

I2C_HandleTypeDef* device_hi2c_mapping[ROBOTTO_DEVICE_COUNT] = {
	[ROBOTTO_DEVICE_LEFT_ENCODER]	=	&hi2c1,
	[ROBOTTO_DEVICE_RIGHT_ENCODER] 	=	&hi2c2,
	[ROBOTTO_DEVICE_IMU] 			=	&hi2c1,
};

SemaphoreHandle_t* device_mutex_mapping[ROBOTTO_DEVICE_COUNT] = {
	[ROBOTTO_DEVICE_LEFT_ENCODER]	=	&i2c1_mutex,
	[ROBOTTO_DEVICE_RIGHT_ENCODER] 	=	&i2c2_mutex,
	[ROBOTTO_DEVICE_IMU] 			=	&i2c1_mutex,
};

void initializeI2CMutexes()
{
	i2c1_mutex = xSemaphoreCreateMutex();
	i2c2_mutex = xSemaphoreCreateMutex();
}

RobottoErrorCode ReadI2C(RobottoI2CDevice device, uint16_t device_address, uint16_t memory_address, uint8_t *data, uint16_t size)
{
	if(xSemaphoreTake(*device_mutex_mapping[device], MUTEX_BLOCK_TICKS) != pdTRUE)
	{
		return ROBOTTO_ERROR;
	}

	HAL_StatusTypeDef read_result = HAL_I2C_Mem_Read(device_hi2c_mapping[device], device_address << 1, memory_address, I2C_MEMADD_SIZE_8BIT, data, size, I2C_TIMEOUT);

	if (xSemaphoreGive(*device_mutex_mapping[device]) != pdTRUE)
	{
		return ROBOTTO_ERROR;
	}

	if (HAL_OK != read_result)
	{
		return ROBOTTO_ERROR;
	}
	return ROBOTTO_OK;
}

