/*
 * i2c_busses.h
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */

#ifndef INCLUDE_I2C_BUSSES_H_
#define INCLUDE_I2C_BUSSES_H_


#include "robotto_common.h"

typedef enum {
	ROBOTTO_DEVICE_LEFT_ENCODER,
	ROBOTTO_DEVICE_RIGHT_ENCODER,
	ROBOTTO_DEVICE_IMU,
	ROBOTTO_DEVICE_COUNT,
} RobottoI2CDevice;

void initializeI2CMutexes();

RobottoErrorCode ReadI2C(RobottoI2CDevice device, uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

#endif /* INCLUDE_I2C_BUSSES_H_ */
