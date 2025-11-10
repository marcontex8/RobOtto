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
    float lastAngle;
    float lastSpeed;
    TickType_t lastTick;
    bool initialized;
} WheelStatus;



bool updateWheelStatus(WheelStatus* encoder, float angle, TickType_t tick);


#endif /* INC_ODOMETRY_H_ */
