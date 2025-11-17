/*
 * pid_motor_controller.c
 *
 *  Created on: Nov 16, 2025
 *      Author: marco
 */
#include "pid_motor_controller.h"


typedef struct{
	float kP;
	float kI;
	float kD;
	float integrated_error;
} PIDStatus;


float calculatePIDResponse(PIDStatus * pid_status, float current_value, float set_point)
{
	float error = set_point - current_value;
	pid_status->integrated_error += error;
	return error * pid_status->kP + pid_status->integrated_error * pid_status->kI;
}


RobottoErrorCode calculateRequiredDutyCycle(const WheelSpeedSetPoint* speed_set_point, const WheelStatus* left_wheel_status, const WheelStatus* right_wheel_status, float* out_left_duty, float* out_right_duty)
{
	// max speed ~= 20rad/s

	static PIDStatus pid_left =  {.kP = 0.1, .kI = 0.1, .kD = 0.0, .integrated_error = 0.0};
	static PIDStatus pid_right = {.kP = 0.1, .kI = 0.1, .kD = 0.0, .integrated_error = 0.0};

	*out_left_duty = calculatePIDResponse(&pid_left, left_wheel_status->last_speed, speed_set_point->left);
	*out_right_duty = calculatePIDResponse(&pid_right, right_wheel_status->last_speed, speed_set_point->right);

	return ROBOTTO_OK;
}
