/*
 * encoder_reader.c
 *
 *  Created on: Oct 30, 2025
 *      Author: marco
 */

#include "encoder_reader.h"
#include "i2c_busses.h"

#include <math.h>

#include "FreeRTOS.h"
#include "semphr.h"


#define I2C_AS5600_ADDRESS 0x36
#define AS5600_ANGLE 0x0E
#define AS5600_STATUS 0x0B
#define AS5600_AGC 0x1A

RobottoI2CDevice getEncoderDevice(Wheel wheel)
{
	if (WHEEL_LEFT == wheel)
	{
		return ROBOTTO_DEVICE_LEFT_ENCODER;
	}
	else
	{
		return ROBOTTO_DEVICE_RIGHT_ENCODER;
	}
}

RobottoErrorCode readAngleRad(Wheel wheel, float* out)
{
    uint8_t buffer[2];
	if (ROBOTTO_OK != ReadI2C(getEncoderDevice(wheel), I2C_AS5600_ADDRESS, AS5600_ANGLE, buffer, 2))
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

	RobottoI2CDevice device = getEncoderDevice(wheel);

	if (ReadI2C(device, I2C_AS5600_ADDRESS, AS5600_STATUS, &status, 1) != ROBOTTO_OK
		|| ReadI2C(device, I2C_AS5600_ADDRESS, AS5600_AGC, &automatic_gain_control, 1) != ROBOTTO_OK
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
