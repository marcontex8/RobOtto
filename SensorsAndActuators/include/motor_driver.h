/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H
#define SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H


#include "sensors_and_actuators_common.h"


void initializeMotorDriver();

void setMotorDutyCycle(Wheel wheel, float normalized_duty_cycle);

#endif /* SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H */
