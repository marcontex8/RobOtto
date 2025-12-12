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

#include "wheel_status_estimator.h"
#include "pid_motor_controller.h"
#include "robotto_common.h"
#include "SEGGER_RTT.h"
#include "queue.h"



extern QueueHandle_t wheels_speed_set_points_queue_handle;
extern QueueHandle_t wheels_status_queue_handle;
static const char* last_error = NULL;




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
	EncoderStatus right_encoder_status = {0};

	if(ROBOTTO_OK != readFullEncoder(WHEEL_LEFT, &left_encoder_status))
	{
		last_error = "INITIALIZATION ERROR, IMPOSSIBLE TO READ LEFT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};
	if(ROBOTTO_OK != readFullEncoder(WHEEL_RIGHT, &right_encoder_status))
	{
		last_error = "INITIALIZATION ERROR, IMPOSSIBLE TO READ RIGHT ENCODER";
		return ACTIVITY_STATUS_ERROR;
	};

	if(0 == left_encoder_status.magnet_detected)
	{
		last_error = "INITIALIZATION ERROR, LEFT MAGNET NOT DETECTED";
		return ACTIVITY_STATUS_ERROR;
	};
	if(0 == right_encoder_status.magnet_detected)
	{
		last_error = "INITIALIZATION ERROR, RIGHT MAGNET NOT DETECTED";
		return ACTIVITY_STATUS_ERROR;
	};


	if(ROBOTTO_OK != readAngleAndUpdateWheelsStatus(left_wheel_status, right_wheel_status))
	{
		last_error = "ERROR WHILE INITIALIZING ENCODERS STATE";
		return ACTIVITY_STATUS_ERROR;
	}

	return ACTIVITY_STATUS_RUNNING;
}




ActivityStatus runWheelsControlStatusRunning(WheelStatus* left_wheel_status, WheelStatus* right_wheel_status)
{
	// GET SETPOINT FROM MOTION PLANNING
	static WheelSpeedSetPoint speed_set_point = {0};
	static int missing_setpoint_counter = 0;

	SEGGER_SYSVIEW_MarkStart(10);
	if (xQueueReceive(wheels_speed_set_points_queue_handle, &speed_set_point, 0) == pdPASS)
	{
		missing_setpoint_counter = 0;
	}
	else
	{
		++missing_setpoint_counter;
	}

	if(missing_setpoint_counter > 6)
	{
		last_error = "Missing Speed SetPoint";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
	}

	SEGGER_SYSVIEW_MarkStop(10);
	// READ WHEEL STATUS FROM ENCODERS
	if(ROBOTTO_OK != readAngleAndUpdateWheelsStatus(left_wheel_status, right_wheel_status))
	{
		last_error = "CANNOT READ FROM ENCODER";
		return ACTIVITY_STATUS_ERROR;
	}


	// RUN CONTROL LOOP AND ACTUATE
	if(!speed_set_point.active)
	{
		setMotorDutyCycle(WHEEL_LEFT, 0);
		setMotorDutyCycle(WHEEL_RIGHT, 0);
		resetController();
	}
	else
	{
		float left_duty, right_duty;
		calculateRequiredDutyCycle(&speed_set_point, left_wheel_status, right_wheel_status, &left_duty, &right_duty);

		setMotorDutyCycle(WHEEL_LEFT, left_duty);
		setMotorDutyCycle(WHEEL_RIGHT, right_duty);
	}

	// SEND UPDATES TO POSE ESTIMATION
	WheelsMovementUpdate wheels_movement_update;
	wheels_movement_update.timestamp = left_wheel_status->last_tick;
	wheels_movement_update.delta_angle_left = left_wheel_status->delta_angle;
	wheels_movement_update.delta_angle_right = right_wheel_status->delta_angle;
	if(pdPASS != xQueueSendToBack(wheels_status_queue_handle, &wheels_movement_update, 0))
	{
		last_error = "wheels_status_queue_handle IS FULL";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
	}
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
		SEGGER_SYSVIEW_ErrorfTarget("%s\n", last_error);
	}
}
