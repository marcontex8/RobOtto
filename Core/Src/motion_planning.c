/*
 * motion_planning.c
 *
 *  Created on: Nov 8, 2025
 *      Author: marco
 */

#include "FreeRTOS.h"
#include "task.h"
#include "robotto_common.h"
#include "queue.h"
#include "imu_reader.h"
#include "SEGGER_RTT.h"

extern QueueHandle_t behavior_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;

extern QueueHandle_t wheels_speed_set_points_queue_handle;

static char* last_error = NULL;





/////////////////////////////////////////////////////////////////
//////////////////////   STATE MACHINE     //////////////////////
/////////////////////////////////////////////////////////////////


#define TEST_SPEED 6.28
#define MAX_ESTIMATED_POSE_DELAY 50

ActivityStatus motionPlanningStatusInit()
{
	RobottoPose estimated_pose;
	if (pdTRUE != xQueuePeek(robotto_pose_queue_handle, &estimated_pose, 0))
	{
		WheelSpeedSetPoint speed_set_point = {0};
		xQueueSend(wheels_speed_set_points_queue_handle, &speed_set_point, 0);

		last_error = "Initializing... waiting for pose estimation.";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
		return ACTIVITY_STATUS_INIT;
	}
	return ACTIVITY_STATUS_RUNNING;
}

ActivityStatus motionPlanningStatusRunning()
{
	uint8_t behavior = 0;
	xQueuePeek(behavior_queue_handle, &behavior, 0);
	RobottoPose estimated_pose;
	if (pdTRUE != xQueuePeek(robotto_pose_queue_handle, &estimated_pose, 0) || (xTaskGetTickCount() - estimated_pose.timestamp) > MAX_ESTIMATED_POSE_DELAY)
	{
		last_error = "Missing updates from pose estimation. Cannot compute motion planning";
		return ACTIVITY_STATUS_ERROR;
	}

	// max speed ~= 20rad/s
	WheelSpeedSetPoint speed_set_point = {0};
	if (behavior == 1)
	{
		speed_set_point.active = true;
		speed_set_point.left = TEST_SPEED;
		speed_set_point.right = TEST_SPEED;
	}
	else if (behavior == 2)
	{
		speed_set_point.active = true;
		speed_set_point.left = TEST_SPEED;
		speed_set_point.right = -TEST_SPEED;
	}
	else if (behavior == 3)
	{
		speed_set_point.active = true;
		speed_set_point.left = -TEST_SPEED;
		speed_set_point.right = TEST_SPEED;
	}
	else
	{
		speed_set_point.active = false;
		speed_set_point.left = 0;
		speed_set_point.right = 0;
	}

	if (xQueueSend(wheels_speed_set_points_queue_handle, &speed_set_point, 0) != pdPASS)
	{
		last_error = "wheels_speed_set_points_queue_handle IS FULL";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
	}
	return ACTIVITY_STATUS_RUNNING;
}



void runMotionPlanningStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = motionPlanningStatusInit();
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = motionPlanningStatusRunning();
	}
	else // ACTIVITY_STATUS_ERROR
	{
		SEGGER_SYSVIEW_ErrorfTarget("%s\n", last_error);
	}


}

