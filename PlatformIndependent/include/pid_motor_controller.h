/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef PLATFORMINDEPENDENT_INCLUDE_PID_MOTOR_CONTROLLER_H
#define PLATFORMINDEPENDENT_INCLUDE_PID_MOTOR_CONTROLLER_H

#include "robotto_common.h"
#include "wheel_status_estimator.h"

void calculateRequiredDutyCycle(const WheelSpeedSetPoint *speed_set_point,
                                const WheelStatus *left_wheel_status,
                                const WheelStatus *right_wheel_status,
                                float *out_left_duty,
                                float *out_right_duty);

void resetController();

#endif /* PLATFORMINDEPENDENT_INCLUDE_PID_MOTOR_CONTROLLER_H */
