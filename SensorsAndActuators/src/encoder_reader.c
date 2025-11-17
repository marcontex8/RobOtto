/*
 * encoder_reader.c
 *
 *  Created on: Oct 30, 2025
 *      Author: marco
 */

#include "encoder_reader.h"
#include <math.h>


extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c2;


I2C_HandleTypeDef* getI2CHandle(Wheel wheel)
{
	if (WHEEL_LEFT == wheel)
	{
		return &hi2c1;
	}
	else
	{
		return &hi2c2;
	}
}

RobottoErrorCode readAngleRad(Wheel wheel, float* out)
{
	uint8_t buffer[2];
	if(HAL_I2C_Mem_Read(getI2CHandle(wheel), 0x36 << 1, 0x0E, 1, buffer, 2, 100) != HAL_OK)
	{
		return ROBOTTO_ERROR;
	}

	uint16_t angle_unsigned = ((buffer[0] & 0x0F) << 8) | buffer[1];
	*out = (angle_unsigned * M_TWOPI) / 4096.0f;


	if (WHEEL_LEFT == wheel)  // left wheel forward direction is counterclockwise compared to it's encoder
	{
		*out = M_TWOPI - *out;
	}
	return ROBOTTO_OK;
}


RobottoErrorCode readFullEncoder(Wheel wheel, EncoderStatus* out)
{
	uint8_t status = 0;
	uint8_t automatic_gain_control = 0;
	float angle = 0.0f;

	I2C_HandleTypeDef* i2c_handle = getI2CHandle(wheel);

	if (HAL_I2C_Mem_Read(i2c_handle, 0x36 << 1, 0x0B, 1, &status, 1, 100) != HAL_OK
		|| HAL_I2C_Mem_Read(i2c_handle, 0x36 << 1, 0x1A, 1, &automatic_gain_control, 1, 100) != HAL_OK
		|| readAngleRad(wheel, &angle) != ROBOTTO_OK)
	{
		return ROBOTTO_ERROR;
	}

	out->too_strong = (status >> 3) & 0x01;
	out->too_weak = (status >> 4) & 0x01;
	out->magnet_detected = (status >> 5) & 0x01;
	out->automatic_gain_control = (automatic_gain_control * 100.0f) / 128.0f;
	out->angle = angle;

	return ROBOTTO_OK;
}
