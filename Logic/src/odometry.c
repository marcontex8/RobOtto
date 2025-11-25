/*
 * odometry.c
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */
#include "odometry.h"
#include "robotto_conf.h"
#include <math.h>


void updateOdometry(const WheelsMovementUpdate* wheels_movement_update, const ImuData* imu_data, RobottoPose* estimated_pose)
{
	float delta_l = wheels_movement_update->delta_angle_left * WHEELS_RADIUS;
	float delta_r = wheels_movement_update->delta_angle_right * WHEELS_RADIUS;
	float delta_s = (delta_r + delta_l) / 2;
	float delta_theta = (delta_r - delta_l) / WHEELS_DISTANCE;

	estimated_pose->x += -1 * delta_s * sinf(estimated_pose->theta + delta_theta/2);
	estimated_pose->y += delta_s * cosf(estimated_pose->theta + delta_theta/2);
	estimated_pose->theta += delta_theta;
}
