/*
 * odometry.c
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */
#include "odometry.h"
#include <math.h>


bool updateWheelStatusEstimation(WheelStatus* wheel_status, float angle, TickType_t tick)
{
    if (!wheel_status->initialized) {
    	wheel_status->last_angle = angle;
    	wheel_status->last_speed = 0.0f;
    	wheel_status->last_tick = tick;
    	wheel_status->initialized = true;

    	return false;
    }

    TickType_t delta_tick = tick - wheel_status->last_tick;
    if (delta_tick == 0)
        return false;

    float delta_angle = angle - wheel_status->last_angle;

    // Handle wrap-around (0–2PI)
    if (delta_angle > M_PI)
        delta_angle -= M_TWOPI;
    else if (delta_angle < -M_PI)
        delta_angle += M_TWOPI;

    // Convert tick difference to seconds
    float delta_time = delta_tick * (portTICK_PERIOD_MS / 1000.0f);

    wheel_status->last_speed = delta_angle / delta_time;
	wheel_status->last_angle = angle;
	wheel_status->last_tick = tick;
    return true;
}
