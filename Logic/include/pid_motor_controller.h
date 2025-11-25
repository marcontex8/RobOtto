/*
 * pid_motor_controller.h
 *
 *  Created on: Nov 16, 2025
 *      Author: marco
 */

#ifndef INC_PID_MOTOR_CONTROLLER_H_
#define INC_PID_MOTOR_CONTROLLER_H_


#include "robotto_common.h"
#include "wheel_status_estimator.h"


void calculateRequiredDutyCycle(const WheelSpeedSetPoint* speed_set_point, const WheelStatus* left_wheel_status, const WheelStatus* right_wheel_status, float* out_left_duty, float* out_right_duty);

void resetController();

#endif /* INC_PID_MOTOR_CONTROLLER_H_ */
