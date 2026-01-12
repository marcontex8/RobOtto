/*
 * object_detection.c
 *
 *  Created on: Dec 26, 2025
 *      Author: marco
 */
#include "robotto_common.h"
#include "ultrasonic_sensor.h"
#include "servo.h"

#include "SEGGER_SYSVIEW.h"
#include "stm32f4xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#include "detection_manager.h"

extern TIM_HandleTypeDef htim10;

static const char* last_error = NULL;

static float servo_angle = 0.0f;
static bool servo_direction = true;


void updateServo()
{
	static unsigned int counter = 0;
	++counter;
	if(counter % 1 == 0)
	{
		if(servo_direction)
		{
			servo_angle += 1.5f;
		}
		else
		{
			servo_angle -= 1.5f;
		}
		if(servo_angle >= 90.0 || servo_angle <= -90.0)
		{
			servo_direction = !servo_direction;
		}
		setServoAngle(servo_angle);
	}
}

ActivityStatus runObjectDetectionInit()
{
	initServo();
	static TickType_t start_time = 0;
	static bool started = false;

	if(!started)
	{
		setServoAngle(servo_angle);
		start_time = xTaskGetTickCount();
		started = true;
	}
	else if(xTaskGetTickCount() - start_time > pdMS_TO_TICKS(3000))
	{
		triggerSensor();
		return ACTIVITY_STATUS_RUNNING;
	}

	return ACTIVITY_STATUS_INIT;
}


ActivityStatus runObjectDetectionRunning()
{
	float distance_m;
	if(ROBOTTO_OK == getMeasurementIfReady(&distance_m))
	{
		addDetection(xTaskGetTickCount(), distance_m, servo_angle);
	}

	triggerSensor();
	updateServo();
	return ACTIVITY_STATUS_RUNNING;
}


void runObjectDetectionStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = runObjectDetectionInit();
		return;
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = runObjectDetectionRunning();
		return;
	}
	else
	{
		// ACTIVITY_STATUS_ERROR
		SEGGER_SYSVIEW_ErrorfTarget("%s\n", last_error);
	}
}
