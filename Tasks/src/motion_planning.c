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

static RobottoPose targetPoses[4] = {
    { .x = 2.0, .y = 2.0 },
    { .x = 0.0, .y = 0.0 },
    { .x = 2.0, .y = 2.0 },
    { .x = 0.0, .y = 0.0 }
};


RobottoPose getNextTargetPose()
{
	static unsigned int next = 0;
	return targetPoses[next++];
	if (next == 4)
	{
		next = 0;
	}
}


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


void planMotion(const RobottoPose* estimated_pose, const RobottoBehavior* behavior, WheelSpeedSetPoint* speed_set_point)
{
	if(ROBOTTO_BEHAVIOR_IDLE == behavior)
	{
		speed_set_point->active = false;
	}
	else
	{
		if(endPoseReached(estimated_pose))
		{
			defineNewTargetPose(getNextTargetPose());
		}

		*speed_set_point = computeWheelSpeedSetpoint(estimated_pose);
		//SEGGER_SYSVIEW_PrintfTarget("Target pose: (x: %d, y: %d, theta: %d)\n", (int)(1000*estimated_pose->x),  (int)(1000*estimated_pose->y),  (int)(1000*estimated_pose->theta));
	}
}


ActivityStatus motionPlanningStatusRunning()
{
	WheelSpeedSetPoint speed_set_point = {0};

	RobottoPose estimated_pose;
	if (pdTRUE != xQueuePeek(robotto_pose_queue_handle, &estimated_pose, 0) || (xTaskGetTickCount() - estimated_pose.timestamp) > MAX_ESTIMATED_POSE_DELAY)
	{
		last_error = "Missing updates from pose estimation. Cannot compute motion planning";
		return ACTIVITY_STATUS_ERROR;
	}

	RobottoBehavior behavior = 0;
	xQueuePeek(behavior_queue_handle, &behavior, 0);

	planMotion(&estimated_pose, &behavior, &speed_set_point);

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
