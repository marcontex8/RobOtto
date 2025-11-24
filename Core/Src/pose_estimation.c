/*
 * pose_estimation.c
 *
 *  Created on: Nov 23, 2025
 *      Author: marco
 */
#include "robotto_common.h"

#include "imu_reader.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "SEGGER_RTT.h"
#include "task.h"



extern QueueHandle_t wheels_status_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;

static char* last_error = NULL;

static ImuData imu_bias = {0};

#define DECIMALS 100000

void removeBias(ImuData* data)
{
	data->acc_x -= imu_bias.acc_x;
	data->acc_y -= imu_bias.acc_y;
	data->acc_z -= imu_bias.acc_z;

	data->gyro_x -= imu_bias.gyro_x;
	data->gyro_y -= imu_bias.gyro_y;
	data->gyro_z -= imu_bias.gyro_z;
}

ActivityStatus poseEstimationStatusInit()
{
	if(ROBOTTO_OK != verifyIMUCommunication())
	{
		last_error = "Impossible to setup IMU communication";
		return ACTIVITY_STATUS_ERROR;
	}
	static unsigned int counter = 0;

	static ImuData current_reading = {0};
	if(ROBOTTO_OK != readIMUData(&current_reading))
	{
		last_error = "Error while reading IMU data";
		return ACTIVITY_STATUS_ERROR;
	}
	imu_bias.acc_x = (imu_bias.acc_x*counter + current_reading.acc_x)/(counter+1);
	imu_bias.acc_y = (imu_bias.acc_y*counter + current_reading.acc_y)/(counter+1);
	imu_bias.acc_z = (imu_bias.acc_z*counter + current_reading.acc_z)/(counter+1);

	imu_bias.gyro_x = (imu_bias.gyro_x*counter + current_reading.gyro_x)/(counter+1);
	imu_bias.gyro_y = (imu_bias.gyro_y*counter + current_reading.gyro_y)/(counter+1);
	imu_bias.gyro_z = (imu_bias.gyro_z*counter + current_reading.gyro_z)/(counter+1);

	++counter;
	if (counter > 100)
	{
		return ACTIVITY_STATUS_RUNNING;
	}
	return ACTIVITY_STATUS_INIT;
}


ActivityStatus poseEstimationStatusRunning()
{
	ImuData imu_data = {0};
	if(ROBOTTO_OK != readIMUData(&imu_data))
	{
		last_error = "Error while reading IMU data";
		return ACTIVITY_STATUS_ERROR;
	}
	removeBias(&imu_data);

	WheelsMovementUpdate wheels_movement_update;
	float travelled_angle_left = 0.0f;
	float travelled_angle_right = 0.0f;

	unsigned int counter = 0;
	while(pdPASS == xQueueReceive(wheels_status_queue_handle, &wheels_movement_update, 0))
	{
		travelled_angle_left += wheels_movement_update.delta_angle_left;
		travelled_angle_right += wheels_movement_update.delta_angle_right;
		++counter;
	}
	if (counter == 0)
	{
		last_error = "NO DATA PROVIDED BY WHEELS CONTROL";
		return ACTIVITY_STATUS_ERROR;
	}

	/*
	TickType_t time = xTaskGetTickCount();
    SEGGER_RTT_printf(0, "\t;%u;\t%d;\t%d;\t%d;\t%d;\t%d\n", time,
    		(int)(DECIMALS*travelled_angle_left),
    		(int)(DECIMALS*travelled_angle_right),
			(int)(DECIMALS*imu_data.gyro_x),
			(int)(DECIMALS*imu_data.gyro_y),
			(int)(DECIMALS*imu_data.gyro_z));
	*/

	RobottoPose estimated_pose = {.timestamp = xTaskGetTickCount(), .x = 10, .y=100, .theta=3.14};
	xQueueOverwrite(robotto_pose_queue_handle, &estimated_pose);

	return ACTIVITY_STATUS_RUNNING;
}

void runPoseEstimationStateMachine()
{
	static ActivityStatus activity_status = ACTIVITY_STATUS_INIT;

	if(ACTIVITY_STATUS_INIT == activity_status)
	{
		activity_status = poseEstimationStatusInit();
	}
	else if(ACTIVITY_STATUS_RUNNING == activity_status)
	{
		activity_status = poseEstimationStatusRunning();
	}
	else // ACTIVITY_STATUS_ERROR
	{
		SEGGER_SYSVIEW_ErrorfTarget("ERROR - PoseEstimationStateMachine: %s\n", last_error);
	}


}
