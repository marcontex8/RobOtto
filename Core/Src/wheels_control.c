/*
 * motor_control.c
 *
 *  Created on: Nov 6, 2025
 *      Author: marco
 */

#include "FreeRTOS.h"
#include "task.h"
#include "encoder_reader.h"
#include "motor_driver.h"

#include "odometry.h"
#include "pid_motor_controller.h"
#include "robotto_common.h"
#include "SEGGER_RTT.h"
#include "queue.h"



extern QueueHandle_t speed_set_points_queue_handle;

static char* last_error = NULL;




RobottoErrorCode readAngleAndUpdateWheelsStatus(WheelStatus* left_wheel_status, WheelStatus* right_wheel_status)
{
	TickType_t tick = xTaskGetTickCount();

	float left_wheel_angle = 0.0f;
	if(ROBOTTO_OK != readAngleRad(WHEEL_LEFT, &left_wheel_angle))
	{
		return ROBOTTO_ERROR;
	}
	updateWheelStatusEstimation(left_wheel_status, left_wheel_angle, tick);


	float right_wheel_angle = 0.0f;
	if(ROBOTTO_OK != readAngleRad(WHEEL_RIGHT, &right_wheel_angle))
	{
		return ROBOTTO_ERROR;
	}
	updateWheelStatusEstimation(right_wheel_status, right_wheel_angle, tick);

	return ROBOTTO_OK;
}



/////////////////////////////////////////////////////////////////
//////////////////////   STATE MACHINE     //////////////////////
/////////////////////////////////////////////////////////////////



ActivityStatus runWheelsControlStatusInit(WheelStatus* left_wheel_status, WheelStatus* right_wheel_status)
{
	initializeMotorDriver();

	EncoderStatus left_encoder_status = {0};
	if(ROBOTTO_OK != readFullEncoder(WHEEL_LEFT, &left_encoder_status) || 0 == left_encoder_status.magnet_detected)
	{
		last_error = "ERROR WHILE READING LEFT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};

	EncoderStatus right_encoder_status = {0};
	if(ROBOTTO_OK != readFullEncoder(WHEEL_RIGHT, &right_encoder_status) || 0 == right_encoder_status.magnet_detected)
	{
		last_error = "ERROR WHILE READING RIGHT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};

	if(ROBOTTO_OK != readAngleAndUpdateWheelsStatus(left_wheel_status, right_wheel_status))
	{
		last_error = "ERROR WHILE UPDATING STATE";
		return ACTIVITY_STATUS_ERROR;
	}

	return ACTIVITY_STATUS_RUNNING;
}

int floatTo3Dec(float input)
{
	return (int)(input*1000);
}


ActivityStatus runWheelsControlStatusRunning(WheelStatus* left_wheel_status, WheelStatus* right_wheel_status)
{
	// GET SETPOINT FROM MOTION PLANNING
	static WheelSpeedSetPoint speed_set_point = {0};
	static int missing_setpoint_counter = 0;


	if (xQueueReceive(speed_set_points_queue_handle, &speed_set_point, 0) == pdPASS)
	{
		missing_setpoint_counter = 0;
	}
	else
	{
		++missing_setpoint_counter;
	}

	if(missing_setpoint_counter > 6)
	{
		last_error = "MISSING UPDATES OF SPEED SET POINTS";
		return ACTIVITY_STATUS_ERROR;
	}


	// READ WHEEL STATUS FROM ENCODERS
	if(ROBOTTO_OK != readAngleAndUpdateWheelsStatus(left_wheel_status, right_wheel_status))
	{
		last_error = "CANNOT READ FROM ENCODER";
		return ACTIVITY_STATUS_ERROR;
	}


	// RUN CONTROL LOOP AND ACTUATE
	float left_duty, right_duty;
	if(calculateRequiredDutyCycle(&speed_set_point, left_wheel_status, right_wheel_status, &left_duty, &right_duty) != ROBOTTO_OK)
	{
		return ACTIVITY_STATUS_ERROR;
	}

	setMotorDutyCycle(WHEEL_LEFT, left_duty);
	setMotorDutyCycle(WHEEL_RIGHT, right_duty);
	/*
	 *
	* c: Print the argument as one char
	* d: Print the argument as a signed integer
	* u: Print the argument as an unsigned integer
	* x: Print the argument as an hexadecimal integer
	* s: Print the string pointed to by the argument
	* p: Print the argument as an 8-digit hexadecimal integer.
	 */
	SEGGER_RTT_printf(0, "%u;%5d;%5d;%5d;%5d;%5d;%5d\n",
			xTaskGetTickCount(),
			floatTo3Dec(speed_set_point.left),
			floatTo3Dec(left_wheel_status->last_speed),
			//floatTo3Dec(left_wheel_status->last_angle),
			floatTo3Dec(left_duty),
			floatTo3Dec(speed_set_point.right),
			floatTo3Dec(right_wheel_status->last_speed),
			//floatTo3Dec(right_wheel_status->last_angle),
			floatTo3Dec(right_duty));



	return ACTIVITY_STATUS_RUNNING;
}




void runWheelsControlStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;
	static WheelStatus left_wheel_status = {0};
	static WheelStatus right_wheel_status = {0};

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = runWheelsControlStatusInit(&left_wheel_status, &right_wheel_status);
		return;
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = runWheelsControlStatusRunning(&left_wheel_status, &right_wheel_status);
		return;
	}
	else
	{
		// ACTIVITY_STATUS_ERROR
		setMotorDutyCycle(WHEEL_RIGHT, 0);
		setMotorDutyCycle(WHEEL_LEFT, 0);
	}
}
