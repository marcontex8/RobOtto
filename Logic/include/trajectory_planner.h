/*
 * trajectory_planner.h
 *
 *  Created on: Dec 30, 2025
 *      Author: marco
 */

#ifndef INCLUDE_TRAJECTORY_PLANNER_H_
#define INCLUDE_TRAJECTORY_PLANNER_H_

#include "robotto_common.h"



WheelSpeedSetPoint computeWheelSpeedSetpoint(RobottoPose current);

void defineNewTrajectory(RobottoPose start, RobottoPose end);


#endif /* INCLUDE_TRAJECTORY_PLANNER_H_ */
