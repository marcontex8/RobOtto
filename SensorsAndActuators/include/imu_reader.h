/*
 * imu_reader.h
 *
 *  Created on: Nov 19, 2025
 *      Author: marco
 */

#ifndef INCLUDE_IMU_READER_H_
#define INCLUDE_IMU_READER_H_

#include "robotto_common.h"
#include <stdint.h>


typedef struct {
    float acc_x;
    float acc_y;
    float acc_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;
} ImuData;

RobottoErrorCode verifyIMUCommunication();

RobottoErrorCode readIMUData(ImuData* out);

#endif /* INCLUDE_IMU_READER_H_ */
