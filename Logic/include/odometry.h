/*
 * odometry.h
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */

#ifndef INCLUDE_ODOMETRY_H_
#define INCLUDE_ODOMETRY_H_

#include "robotto_common.h"

void updateOdometry(const WheelsMovementUpdate* wheels_movement_update, const ImuData* imu_data, RobottoPose* estimated_pose);

#endif /* INCLUDE_ODOMETRY_H_ */
