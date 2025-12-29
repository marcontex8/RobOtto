/*
 * object_detection.c
 *
 *  Created on: Dec 26, 2025
 *      Author: marco
 */


#include "robotto_common.h"
#include "ultrasonic_sensor.h"


#include "SEGGER_SYSVIEW.h"
#include "stm32f4xx_hal.h"
extern TIM_HandleTypeDef htim10;

static const char* last_error = NULL;

ActivityStatus runObjectDetectionInit()
{
	triggerSensor();

	return ACTIVITY_STATUS_RUNNING;
}


ActivityStatus runObjectDetectionRunning()
{
	uint32_t elapsed_time;
	if(ROBOTTO_OK == getMeasurementIfReady(&elapsed_time))
	{
		float distance_m = elapsed_time / 5831.0f;
		unsigned int distance_cm = (unsigned int)(distance_m * 100);
		SEGGER_SYSVIEW_PrintfHost("Distance: %u", distance_cm);
	}
	else
	{
		SEGGER_SYSVIEW_Print("No Measure");
	}
	triggerSensor();
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
