/*
 * odometry.c
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */
#include "odometry.h"


float calculateSpeed(float angle, TickType_t tick)
{
    static float lastAngle = 0.0f;
    static TickType_t lastTick = 0;
    static int firstCall = 1;

    if (firstCall) {
        lastAngle = angle;
        lastTick = tick;
        firstCall = 0;
        return 0.0f; // no speed on first call
    }

    TickType_t deltaTick = tick - lastTick;
    if (deltaTick == 0)
        return 0.0f;

    float deltaAngle = angle - lastAngle;

    // Handle wrap-around (0–360)
    if (deltaAngle > 180.0f)
        deltaAngle -= 360.0f;
    else if (deltaAngle < -180.0f)
        deltaAngle += 360.0f;

    // Convert tick difference to seconds
    float deltaTime = deltaTick * (portTICK_PERIOD_MS / 1000.0f);

    float speed = deltaAngle / deltaTime; // degrees per second

    // Update stored values
    lastAngle = angle;
    lastTick = tick;

    return speed;
}
