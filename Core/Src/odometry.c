/*
 * odometry.c
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */
#include "odometry.h"


bool updateWheelStatus(WheelStatus* encoder, float angle, TickType_t tick)
{
    if (!encoder->initialized) {
    	encoder->lastAngle = angle;
    	encoder->lastSpeed = 0.0f;
    	encoder->lastTick = tick;
    	encoder->initialized = true;

    	return false;
    }

    TickType_t deltaTick = tick - encoder->lastTick;
    if (deltaTick == 0)
        return false;

    float deltaAngle = angle - encoder->lastAngle;

    // Handle wrap-around (0–360)
    if (deltaAngle > 180.0f)
        deltaAngle -= 360.0f;
    else if (deltaAngle < -180.0f)
        deltaAngle += 360.0f;

    // Convert tick difference to seconds
    float deltaTime = deltaTick * (portTICK_PERIOD_MS / 1000.0f);

    encoder->lastSpeed = deltaAngle / deltaTime; // degrees per second
	encoder->lastAngle = angle;
	encoder->lastTick = tick;
    return true;
}
