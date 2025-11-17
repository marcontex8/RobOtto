/*
 * odometry.h
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */

#ifndef INC_ODOMETRY_H_
#define INC_ODOMETRY_H_

#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>

typedef struct{
    float last_angle;
    float last_speed;
    TickType_t last_tick;
    bool initialized;
} WheelStatus;




bool updateWheelStatusEstimation(WheelStatus* wheel_status, float angle, TickType_t tick);


#endif /* INC_ODOMETRY_H_ */
