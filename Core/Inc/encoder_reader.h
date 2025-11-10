/*
 * encoder_reader.h
 *
 *  Created on: Oct 30, 2025
 *      Author: marco
 */

#ifndef INC_ENCODER_READER_H_
#define INC_ENCODER_READER_H_

#include <stdbool.h>

#include "stm32f4xx_hal.h"
#include "robotto_common.h"

typedef struct {
    uint8_t magnet_detected;
    uint8_t too_strong;
    uint8_t too_weak;
    float automatic_gain_control;
    float angle;
} EncoderStatus;


RobottoErrorCode readAngle(I2C_HandleTypeDef* i2c_handle, float* out);
RobottoErrorCode readFullEncoder(I2C_HandleTypeDef* i2c_handle, EncoderStatus* out);

#endif /* INC_ENCODER_READER_H_ */
