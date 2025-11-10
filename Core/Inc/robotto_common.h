/*
 * robotto_common.h
 *
 *  Created on: Nov 7, 2025
 *      Author: marco
 */

#ifndef INC_ROBOTTO_COMMON_H_
#define INC_ROBOTTO_COMMON_H_


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
} WheelSpeedSetPoint;



#endif /* INC_ROBOTTO_COMMON_H_ */
