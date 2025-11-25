/*
 * robotto_common.h
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */

#ifndef INC_ROBOTTO_COMMON_H_
#define INC_ROBOTTO_COMMON_H_


#define TickType_t uint32_t

#include <stdbool.h>
#include <stdint.h>


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

typedef struct {
	TickType_t timestamp;
    float acc_x;
    float acc_y;
    float acc_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;
} ImuData;

#endif /* INC_ROBOTTO_COMMON_H_ */
