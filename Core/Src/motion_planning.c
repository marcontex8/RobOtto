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


extern QueueHandle_t speed_set_points_queue_handle;

static char* last_error = NULL;





/////////////////////////////////////////////////////////////////
//////////////////////   STATE MACHINE     //////////////////////
/////////////////////////////////////////////////////////////////




ActivityStatus motionPlanningStatusInit()
{
	return ACTIVITY_STATUS_RUNNING;
}

ActivityStatus motionPlanningStatusRunning()
{
	WheelSpeedSetPoint speed_set_point = {0};

	static unsigned int setpoint = 0;
	if (setpoint == 1)
	{
		speed_set_point.left = 6.28;
	}
	else if (setpoint == 2)
	{
		speed_set_point.right = 6.28;
	}
	else if (setpoint == 3)
	{
		speed_set_point.left = 6.28;
		speed_set_point.right = 6.28;
	}
	else
	{
		speed_set_point.left = 0;
		speed_set_point.right = 0;
	}
	/*
	static uint32_t counter = 0;
	if(counter < 20U)
	{
		speed_set_point.left = 800;
		speed_set_point.right = 800;
	}
	else if (counter < 40U)
	{
		speed_set_point.left = 0;
		speed_set_point.right = 0;
	}
	else if (counter < 60U)
	{
		speed_set_point.left = -800;
		speed_set_point.right =  800;
	}
	else if (counter < 80U)
	{
		speed_set_point.left =  800;
		speed_set_point.right = -800;
	}
	else if (counter < 100U)
	{
		speed_set_point.left = -800;
		speed_set_point.right = -800;
	}
	else
	{
		counter = 0;
	}
	counter++;
	*/

	if (xQueueSend(speed_set_points_queue_handle, &speed_set_point, 0) != pdPASS)
	{
		last_error = "ERROR WHILE PUSHING TO THE QUEUE";
		return ACTIVITY_STATUS_ERROR;
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

	}


}

