/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef LOGIC_INCLUDE_TRAJECTORY_PLANNER_H
#define LOGIC_INCLUDE_TRAJECTORY_PLANNER_H

#include "robotto_common.h"


const RobottoPose* getCurrentTargetPose();

WheelSpeedSetPoint computeWheelSpeedSetpoint(const RobottoPose* current);

void defineNewTargetPose(RobottoPose end);

bool endPoseReached(const RobottoPose* current);

#endif /* LOGIC_INCLUDE_TRAJECTORY_PLANNER_H */
