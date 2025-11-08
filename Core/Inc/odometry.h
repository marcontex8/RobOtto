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

float calculateSpeed(float angle, TickType_t tick);


#endif /* INC_ODOMETRY_H_ */
