/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "trajectory_planner.h"

#include "robotto_common.h"
#include "robotto_conf.h"

#include "logic_common.h"
#include <math.h>

#define CRUISE_SPEED 6.28f * WHEELS_RADIUS
#define POSITION_TOLERANCE 0.05f
#define LOOKAHEAD_DISTANCE 0.4f
#define K_P_ANGULAR 1.0f

static RobottoPose end_pose = {0};

const RobottoPose* getCurrentTargetPose()
{
	return &end_pose;
}

bool endPoseReached(const RobottoPose* current)
{
	float euclidean_distance = sqrtf((current->x - end_pose.x)*(current->x - end_pose.x) + (current->y - end_pose.y) * (current->y - end_pose.y));
	if(euclidean_distance <= POSITION_TOLERANCE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void defineNewTargetPose(RobottoPose end)
{
	end_pose = end;
}

WheelSpeedSetPoint computeWheelSpeedSetpoint(const RobottoPose* current)
{
    WheelSpeedSetPoint out = {0};

    float target_vector_x = end_pose.x - current->x;
    float target_vector_y = end_pose.y - current->y;
    float distance = sqrtf(target_vector_x * target_vector_x +
                           target_vector_y * target_vector_y);

    float linear_speed = CRUISE_SPEED;

    if (distance < POSITION_TOLERANCE)
    {
        out.active = false;
        out.left = 0.0f;
        out.right = 0.0f;
    }
    else
    {
		float scale = fminf(1.0f, LOOKAHEAD_DISTANCE / distance);
		float lookahead_x = current->x + target_vector_x * scale;
		float lookahead_y = current->y + target_vector_y * scale;

		float alpha = wrapToPI(atan2f(current->x - lookahead_x, lookahead_y - current->y) - current->theta);
		float ld = fmaxf(LOOKAHEAD_DISTANCE * scale, 1e-6f);
		float curvature = 2.0f * sinf(alpha) / ld;
		float angular_speed = K_P_ANGULAR * linear_speed * curvature;

		float v_left  = linear_speed - (angular_speed * WHEELS_DISTANCE / 2.0f);
		float v_right = linear_speed + (angular_speed * WHEELS_DISTANCE / 2.0f);

		out.active = true;
		out.left  = v_left  / WHEELS_RADIUS;
		out.right = v_right / WHEELS_RADIUS;
	}

    return out;
}

