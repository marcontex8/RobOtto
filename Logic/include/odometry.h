/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef LOGIC_INCLUDE_ODOMETRY_H
#define LOGIC_INCLUDE_ODOMETRY_H

#include "robotto_common.h"

void updateOdometry(const WheelsMovementUpdate* wheels_movement_update, const ImuData* imu_data, RobottoPose* estimated_pose);

#endif /* LOGIC_INCLUDE_ODOMETRY_H */
