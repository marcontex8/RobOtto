/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "FreeRTOS.h"

#include "task.h"
#include "robotto_common.h"
#include "queue.h"
#include "imu_reader.h"
#include <math.h>
#include "trajectory_planner.h"
#include "SEGGER_SYSVIEW.h"

extern QueueHandle_t behavior_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;
extern QueueHandle_t wheels_speed_set_points_queue_handle;

extern QueueHandle_t robotto_motion_telemetry_queue_handle;

static const char* last_error = NULL;

#define MAX_ESTIMATED_POSE_DELAY 50

static RobottoPose targetPoses[4] = {
    { .x = 0.0, .y = 1.0 },
    { .x = 1.0, .y = 1.0 },
    { .x = 1.0, .y = 0.0 },
    { .x = 0.0, .y = 0.0 }
};


RobottoPose getNextTargetPose()
{
	static unsigned int next = 0;
	RobottoPose pose = targetPoses[next];
	if (++next == 4)
	{
		next = 0;
	}
	return pose;
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


ActivityStatus motionPlanningStatusRunning()
{
	WheelSpeedSetPoint speed_set_point = {0};

	RobottoPose estimated_pose;
	if (pdTRUE != xQueuePeek(robotto_pose_queue_handle, &estimated_pose, 0) || (xTaskGetTickCount() - estimated_pose.timestamp) > MAX_ESTIMATED_POSE_DELAY)
	{
		last_error = "Missing updates from pose estimation. Cannot compute motion planning";
		return ACTIVITY_STATUS_ERROR;
	}

	RobottoBehavior behavior = ROBOTTO_BEHAVIOR_IDLE;
	xQueuePeek(behavior_queue_handle, &behavior, 0);

	if(ROBOTTO_BEHAVIOR_IDLE == behavior)
	{
		speed_set_point.active = false;
	}
	else
	{
		if(endPoseReached(&estimated_pose))
		{
			defineNewTargetPose(getNextTargetPose());
		}

		speed_set_point = computeWheelSpeedSetpoint(&estimated_pose);
	}
	if (xQueueSend(wheels_speed_set_points_queue_handle, &speed_set_point, 0) != pdPASS)
	{
		last_error = "wheels_speed_set_points_queue_handle IS FULL";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
	}

	RobottoMotionTelemetry motion_telemetry = {
			.pose = estimated_pose,
			.target_pose = *getCurrentTargetPose(),
			.speed_set_point = speed_set_point
	};
	xQueueOverwrite(robotto_motion_telemetry_queue_handle, &motion_telemetry);
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
