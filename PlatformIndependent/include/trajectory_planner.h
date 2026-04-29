/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef PLATFORMINDEPENDENT_INCLUDE_TRAJECTORY_PLANNER_H
#define PLATFORMINDEPENDENT_INCLUDE_TRAJECTORY_PLANNER_H

#include "robotto_common.h"

const RobottoPose *getCurrentTargetPose();

WheelSpeedSetPoint computeWheelSpeedSetpoint(const RobottoPose *current);

void defineNewTargetPose(RobottoPose end);

bool endPoseReached(const RobottoPose *current);

#endif /* PLATFORMINDEPENDENT_INCLUDE_TRAJECTORY_PLANNER_H */
