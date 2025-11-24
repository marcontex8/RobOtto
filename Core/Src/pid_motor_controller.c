/*
 * pid_motor_controller.c
 *
 *  Created on: Nov 16, 2025
 *      Author: marco
 */
#include "pid_motor_controller.h"
#include "SEGGER_RTT.h"

// ASSUME FIXED 10ms PERIOD
#define PERIOD 0.01


typedef struct{
	float kP;
	float kI;
	float kD;
	float integrated_error;
} PIDStatus;

static PIDStatus pid_left =  {.kP = 0.1, .kI = 1, .kD = 0.0, .integrated_error = 0.0};
static PIDStatus pid_right = {.kP = 0.1, .kI = 1, .kD = 0.0, .integrated_error = 0.0};

float calculatePIDResponse(PIDStatus * pid_status, float actual_speed, float desired_speed)
{
	float error = desired_speed - actual_speed;

	float proportional = error * pid_status->kP;
	float integral = pid_status->integrated_error + pid_status->kI * error * PERIOD;

	float output = proportional + integral;

	// anti-windup
	if (output > 1.0)
	{
		output = 1.0;
	}
	else if(output < -1.0)
	{
		output = -1.0;
	}
	else
	{
		pid_status->integrated_error = integral;
	}
	return output;
}

void calculateRequiredDutyCycle(const WheelSpeedSetPoint* speed_set_point, const WheelStatus* left_wheel_status, const WheelStatus* right_wheel_status, float* out_left_duty, float* out_right_duty)
{
	*out_left_duty = calculatePIDResponse(&pid_left, left_wheel_status->filtered_speed, speed_set_point->left);
	*out_right_duty = calculatePIDResponse(&pid_right, right_wheel_status->filtered_speed, speed_set_point->right);
}


void resetController()
{
	pid_left.integrated_error = 0.0;
	pid_right.integrated_error = 0.0;
}
