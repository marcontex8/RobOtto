/*
 * trajectory_planner.h
 *
 *  Created on: Dec 30, 2025
 *      Author: marco
 */

#ifndef INCLUDE_TRAJECTORY_PLANNER_H_
#define INCLUDE_TRAJECTORY_PLANNER_H_

#include "robotto_common.h"


const RobottoPose* getCurrentTargetPose();

WheelSpeedSetPoint computeWheelSpeedSetpoint(const RobottoPose* current);

void defineNewTargetPose(RobottoPose end);

bool endPoseReached(const RobottoPose* current);

#endif /* INCLUDE_TRAJECTORY_PLANNER_H_ */
