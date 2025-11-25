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


RobottoErrorCode verifyIMUCommunication();

RobottoErrorCode readIMUData(ImuData* out);

#endif /* INCLUDE_IMU_READER_H_ */
