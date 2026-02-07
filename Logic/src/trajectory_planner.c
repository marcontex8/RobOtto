/*
 * trajectory_planner.c
 *
 *  Created on: Dec 30, 2025
 *      Author: marco
 */

#include "trajectory_planner.h"
#include "robotto_common.h"
#include "robotto_conf.h"

#include "logic_common.h"
#include <math.h>

#define CRUISE_SPEED 6.28f
#define POSITION_TOLERANCE 0.05f
#define LOOKAHEAD_DISTANCE 0.4f
#define K_P_ANGULAR 1.0f

static RobottoPose end_pose = {0};

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
/*
		if(alpha > 0.9 * M_PI)
		{
			out.active = true;
			out.left  = -1.0 * CRUISE_SPEED / WHEELS_RADIUS;
			out.right = CRUISE_SPEED / WHEELS_RADIUS;
		}
		else if(alpha < -0.9 * M_PI)
		{
			out.active = true;
			out.left  = CRUISE_SPEED / WHEELS_RADIUS;
			out.right = -1.0 * CRUISE_SPEED / WHEELS_RADIUS;
		}
		else
*/
		{
			float ld = fmaxf(LOOKAHEAD_DISTANCE * scale, 1e-6f);
			float curvature = 2.0f * sinf(alpha) / ld;
			float angular_speed = K_P_ANGULAR * linear_speed * curvature;

			float v_left  = linear_speed - (angular_speed * WHEELS_DISTANCE / 2.0f);
			float v_right = linear_speed + (angular_speed * WHEELS_DISTANCE / 2.0f);

			/* Optional min-speed ramp (uncomment if needed)
			float min_allowed = (slow_radius > 0.0f)
				? (distance >= slow_radius ? min_speed : min_speed * (distance / slow_radius))
				: min_speed;

			if (fabsf(v_left)  > 0.0f && fabsf(v_left)  < min_allowed) v_left  = copysignf(min_allowed, v_left);
			if (fabsf(v_right) > 0.0f && fabsf(v_right) < min_allowed) v_right = copysignf(min_allowed, v_right);
			*/

			out.active = true;
			out.left  = v_left  / WHEELS_RADIUS;
			out.right = v_right / WHEELS_RADIUS;
		}
	}
    return out;
}

