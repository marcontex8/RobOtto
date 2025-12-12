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

#include "odometry.h"

extern QueueHandle_t wheels_status_queue_handle;
extern QueueHandle_t robotto_pose_queue_handle;

static const char* last_error = NULL;

static ImuData imu_bias = {0};
#define MEASURES_FOR_BIAS_ESTIMATION 100

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


void addMeasureToBias(ImuData* imu_bias, unsigned int counter, const ImuData* current_reading)
{
	imu_bias->acc_x = (imu_bias->acc_x*counter + current_reading->acc_x)/(counter+1);
	imu_bias->acc_y = (imu_bias->acc_y*counter + current_reading->acc_y)/(counter+1);
	imu_bias->acc_z = (imu_bias->acc_z*counter + current_reading->acc_z)/(counter+1);

	imu_bias->gyro_x = (imu_bias->gyro_x*counter + current_reading->gyro_x)/(counter+1);
	imu_bias->gyro_y = (imu_bias->gyro_y*counter + current_reading->gyro_y)/(counter+1);
	imu_bias->gyro_z = (imu_bias->gyro_z*counter + current_reading->gyro_z)/(counter+1);
}


ActivityStatus poseEstimationStatusInit()
{
	static unsigned int counter = 0;
	++counter;
	if (counter <= MEASURES_FOR_BIAS_ESTIMATION)
	{
		ImuData current_reading = {0};
		if(ROBOTTO_OK != readIMUData(&current_reading))
		{
			last_error = "Error while reading IMU data";
			return ACTIVITY_STATUS_ERROR;
		}
		addMeasureToBias(&imu_bias, counter, &current_reading);

		return ACTIVITY_STATUS_INIT;
	}

	WheelsMovementUpdate wheels_movement_update;
	if (pdPASS != xQueueReceive(wheels_status_queue_handle, &wheels_movement_update, 0))
	{
		last_error = "Initializing... waiting for wheel status";
		SEGGER_SYSVIEW_WarnfTarget("%s\n", last_error);
		return ACTIVITY_STATUS_INIT;
	}
	return ACTIVITY_STATUS_RUNNING;

}


ActivityStatus poseEstimationStatusRunning()
{
	ImuData imu_data = {0};
	if(ROBOTTO_OK != readIMUData(&imu_data))
	{
		last_error = "Impossible to read IMU data";
		return ACTIVITY_STATUS_ERROR;
	}
	removeBias(&imu_data);


	if(0 == uxQueueMessagesWaiting(wheels_status_queue_handle))
	{
		last_error = "No wheels encoders data available";
		return ACTIVITY_STATUS_ERROR;
	}

	static RobottoPose estimated_pose = {0};
	WheelsMovementUpdate wheels_movement_update;
	while(pdPASS == xQueueReceive(wheels_status_queue_handle, &wheels_movement_update, 0))
	{
		updateOdometry(&wheels_movement_update, &imu_data, &estimated_pose);
	}
	estimated_pose.timestamp = xTaskGetTickCount();

    SEGGER_RTT_printf(0, "\t\t\t;%u;\t\t\t%d;\t\t\t%d;\t\t\t%d\n",
    		estimated_pose.timestamp,
			(int)(DECIMALS*estimated_pose.x),
			(int)(DECIMALS*estimated_pose.y),
			(int)(DECIMALS*estimated_pose.theta));
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
		SEGGER_SYSVIEW_ErrorfTarget("%s\n", last_error);
	}


}
