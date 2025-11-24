/*
 * robotto_common.h
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */

#ifndef INC_ROBOTTO_COMMON_H_
#define INC_ROBOTTO_COMMON_H_

#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>

// wheel radius [m]
#define WHEELS_RADIUS 0.0334

// maximum angular wheel speed achievable without load [rad/s]
#define MAX_ANGULAR_WHEEL_SPEED 20.7

// distance between the wheels (width)
#define WHEELS_DISTANCE 0.186



typedef enum {
	ROBOTTO_OK = 0,
	ROBOTTO_ERROR = 1,
} RobottoErrorCode;


typedef enum {
	ACTIVITY_STATUS_INIT = 0,
	ACTIVITY_STATUS_RUNNING,
	ACTIVITY_STATUS_ERROR
} ActivityStatus;


typedef struct {
	float left;
	float right;
	bool active;
} WheelSpeedSetPoint;


typedef struct {
	TickType_t timestamp;
	float delta_angle_left;
	float delta_angle_right;
} WheelsMovementUpdate;

typedef struct {
	TickType_t timestamp;
	float x;
	float y;
	float theta;
} RobottoPose;

#endif /* INC_ROBOTTO_COMMON_H_ */
