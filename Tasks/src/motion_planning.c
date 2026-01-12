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
#include <math.h>
#include "trajectory_planner.h"

extern QueueHandle_t behavior_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;

extern QueueHandle_t wheels_speed_set_points_queue_handle;

static const char* last_error = NULL;

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
	RobottoPose estimated_pose;
	if (pdTRUE != xQueuePeek(robotto_pose_queue_handle, &estimated_pose, 0) || (xTaskGetTickCount() - estimated_pose.timestamp) > MAX_ESTIMATED_POSE_DELAY)
	{
		last_error = "Missing updates from pose estimation. Cannot compute motion planning";
		return ACTIVITY_STATUS_ERROR;
	}

	static RobottoBehavior last_behavior = 0;
	RobottoBehavior new_behavior = 0;
	xQueuePeek(behavior_queue_handle, &new_behavior, 0);
	if(new_behavior != last_behavior && ROBOTTO_BEHAVIOR_IDLE != new_behavior)
	{
		RobottoPose end_pose = estimated_pose;
		end_pose.y += 2.0f;
		defineNewTrajectory(estimated_pose, end_pose);
	}

	WheelSpeedSetPoint speed_set_point = computeWheelSpeedSetpoint(estimated_pose);

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

