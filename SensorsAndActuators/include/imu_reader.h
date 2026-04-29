/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef SENSORSANDACTUATORS_INCLUDE_IMU_READER_H
#define SENSORSANDACTUATORS_INCLUDE_IMU_READER_H

#include "robotto_common.h"
#include <stdint.h>


RobottoErrorCode verifyIMUCommunication();

RobottoErrorCode readIMUData(ImuData* out);

#endif /* SENSORSANDACTUATORS_INCLUDE_IMU_READER_H */
