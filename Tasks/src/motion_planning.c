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

extern QueueHandle_t behavior_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;

extern QueueHandle_t wheels_speed_set_points_queue_handle;

static char* last_error = NULL;





/////////////////////////////////////////////////////////////////
//////////////////////   STATE MACHINE     //////////////////////
/////////////////////////////////////////////////////////////////


#define CRUISE_SPEED 6.28
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

typedef enum{
	MOTION_PHASE_INIT,
	MOTION_PHASE_ACCELERATION,
	MOTION_PHASE_CRUISE,
	MOTION_PHASE_DECELERATION,
	MOTION_PHASE_STOP,
} MotionPhase;

void makeCircle(const RobottoPose* estimated_pose, WheelSpeedSetPoint* speed_set_point)
{
	static unsigned int phase = 0;
	static float start_angle = 0.0f;
	static float destination_angle = 0.0f;

	static unsigned int ramp_step = 1;
	static unsigned int ramp_max = 20;
	static unsigned int current_ramp_level = 0;

	if(phase == MOTION_PHASE_INIT)
	{
		start_angle = estimated_pose->theta;
		destination_angle = start_angle + M_PI;
		phase = MOTION_PHASE_ACCELERATION;
	}
	else if (phase == MOTION_PHASE_ACCELERATION)
	{
		speed_set_point->active = true;
		speed_set_point->left = -CRUISE_SPEED * ((float)current_ramp_level/(float)ramp_max);
		speed_set_point->right = CRUISE_SPEED* ((float)current_ramp_level/(float)ramp_max);

		current_ramp_level += ramp_step;
		if(current_ramp_level >= ramp_max)
		{
			phase = MOTION_PHASE_CRUISE;
		}
	}
	else if(phase == MOTION_PHASE_CRUISE)
	{
		speed_set_point->active = true;
		speed_set_point->left = -CRUISE_SPEED;
		speed_set_point->right = CRUISE_SPEED;

		if(fabs(estimated_pose->theta - destination_angle) < 0.2)
		{
			phase = MOTION_PHASE_DECELERATION;
		}
	}
	else if(phase == MOTION_PHASE_DECELERATION)
	{
		speed_set_point->active = true;
		speed_set_point->left = -CRUISE_SPEED * (1.0f - (float)current_ramp_level/(float)ramp_max);
		speed_set_point->right = CRUISE_SPEED* (1.0f - (float)current_ramp_level/(float)ramp_max);

		current_ramp_level += ramp_step;
		if(current_ramp_level < ramp_step)
		{
			phase = MOTION_PHASE_STOP;
		}
	}
	else // MOTION_PHASE_STOP
	{
		speed_set_point->active = false;
		speed_set_point->left = 0;
		speed_set_point->right = 0;
	}
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

	WheelSpeedSetPoint speed_set_point = {0};
	if(behavior == 1)
	{
		static int counter = 0;
		if(counter < 120)
		{
			speed_set_point.active = true;
			speed_set_point.left = 0;//CRUISE_SPEED;
			speed_set_point.right = CRUISE_SPEED;

			++counter;
		}
	}

	//makeCircle(&estimated_pose, &speed_set_point);

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

