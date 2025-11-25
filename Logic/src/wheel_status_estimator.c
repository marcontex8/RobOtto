/*
 * wheel_status_estimator.c
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */
#include "wheel_status_estimator.h"
#include <math.h>

#define SMOOTHING_FACTOR 0.3



bool updateWheelStatusEstimation(WheelStatus* wheel_status, float angle, TickType_t tick)
{
    if (!wheel_status->initialized) {
    	wheel_status->last_angle = angle;
    	wheel_status->filtered_speed = 0.0f;
    	wheel_status->delta_angle = 0.0f;
    	wheel_status->last_tick = tick;
    	wheel_status->initialized = true;

    	return false;
    }

    TickType_t delta_tick = tick - wheel_status->last_tick;
    if (delta_tick == 0)
        return false;

    wheel_status->delta_angle = angle - wheel_status->last_angle;

    // Handle wrap-around (0–2PI)
    if (wheel_status->delta_angle > M_PI)
    	wheel_status->delta_angle -= M_TWOPI;
    else if (wheel_status->delta_angle < -M_PI)
    	wheel_status->delta_angle += M_TWOPI;

    // Convert tick difference to seconds
    float delta_time = delta_tick * (portTICK_PERIOD_MS / 1000.0f);
    wheel_status->last_speed = wheel_status->delta_angle / delta_time;
    wheel_status->filtered_speed = SMOOTHING_FACTOR * wheel_status->last_speed + (1-SMOOTHING_FACTOR) * wheel_status->filtered_speed;
	wheel_status->last_angle = angle;
	wheel_status->last_tick = tick;
    return true;
}
