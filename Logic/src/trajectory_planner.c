/*
 * trajectory_planner.c
 *
 *  Created on: Dec 30, 2025
 *      Author: marco
 */

#include "trajectory_planner.h"
#include "robotto_common.h"
#include "robotto_conf.h"

#include <math.h>

#define CRUISE_SPEED 6.28

static RobottoPose start_pose;
static RobottoPose end_pose;

void defineNewTrajectory(RobottoPose start, RobottoPose end)
{
	start_pose = start;
	end_pose = end;
}

float normalize_angle(float a)
{
    while (a > M_PI)  a -= 2.0f * M_PI;
    while (a < -M_PI) a += 2.0f * M_PI;
    return a;
}


WheelSpeedSetPoint computeWheelSpeedSetpoint(RobottoPose current)
{
	/*
    // Controller gains
    const float k_rho   = 0.8f;
    const float k_alpha = 2.5f;
    const float k_beta  = -0.8f;

    float dx = end_pose.x - current.x;
    float dy = end_pose.y - current.y;

    float rho   = sqrtf(dx*dx + dy*dy);
    float alpha = atan2f(dy, dx) - current.theta;
    float beta  = end_pose.theta - current.theta - alpha;

    alpha = normalize_angle(alpha);
    beta  = normalize_angle(beta);

    // Unicycle commands
    float v = k_rho * rho;
    float w = k_alpha * alpha + k_beta * beta;

    WheelSpeedSetPoint setpoint = {.active = true, .left = (v - (WHEELS_DISTANCE * 0.5f) * w), .right = (v + (WHEELS_DISTANCE * 0.5f) * w)};
    */
    WheelSpeedSetPoint setpoint = {.active = false, .left = 6.0, .right = 4.0};
	return setpoint;
}

