/*
 * encoder_reader.c
 *
 *  Created on: Oct 30, 2025
 *      Author: marco
 */

#include "encoder_reader.h"

bool read_angle(I2C_HandleTypeDef* ic2_handle, float* out)
{
	uint8_t buffer[2];
	if(HAL_I2C_Mem_Read(ic2_handle, 0x36 << 1, 0x0E, 1, buffer, 2, 100) != HAL_OK)
	{
		return false;
	}
	uint16_t angle_unsigned = ((buffer[0] & 0x0F) << 8) | buffer[1];
	*out = (angle_unsigned * 360.0f) / 4096.0f;
	return true;
}


bool read_full_encoder(I2C_HandleTypeDef* ic2_handle, encoder_reading_t* out)
{
	uint8_t status = 0;
	uint8_t automatic_gain_control = 0;
	float angle = 0.0f;

	bool reading = (HAL_I2C_Mem_Read(ic2_handle, 0x36 << 1, 0x0B, 1, &status, 1, 100) == HAL_OK);
	reading = reading && (HAL_I2C_Mem_Read(ic2_handle, 0x36 << 1, 0x1A, 1, &automatic_gain_control, 1, 100) == HAL_OK);
	reading = reading && read_angle(ic2_handle, &angle);
	if (!reading)
	{
		return false;
	}

	out->too_strong = (status >> 3) & 0x01;
	out->too_weak = (status >> 4) & 0x01;
	out->magnet_detected = (status >> 5) & 0x01;
	out->automatic_gain_control = (automatic_gain_control * 100.0f) / 128.0f;
	out->angle = angle;

	return true;
}
