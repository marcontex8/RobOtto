/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef SENSORSANDACTUATORS_INCLUDE_ULTRASONIC_SENSOR_H
#define SENSORSANDACTUATORS_INCLUDE_ULTRASONIC_SENSOR_H

#include "robotto_common.h"

void triggerSensor(void);

RobottoErrorCode getMeasurementIfReady(float *distance);

#endif /* SENSORSANDACTUATORS_INCLUDE_ULTRASONIC_SENSOR_H */
