/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef COMMON_INCLUDE_I2C_BUSSES_H
#define COMMON_INCLUDE_I2C_BUSSES_H

#include "robotto_common.h"

typedef enum
{
    ROBOTTO_DEVICE_LEFT_ENCODER,
    ROBOTTO_DEVICE_RIGHT_ENCODER,
    ROBOTTO_DEVICE_IMU,
    ROBOTTO_DEVICE_COUNT,
} RobottoI2CDevice;

void initializeI2CMutexes();

RobottoErrorCode ReadI2C(RobottoI2CDevice device,
                         uint16_t DevAddress,
                         uint16_t MemAddress,
                         uint8_t *pData,
                         uint16_t Size);

#endif /* COMMON_INCLUDE_I2C_BUSSES_H */
