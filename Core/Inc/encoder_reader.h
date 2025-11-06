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

extern I2C_HandleTypeDef hi2c2;


typedef struct {
    uint8_t magnet_detected;
    uint8_t too_strong;
    uint8_t too_weak;
    float automatic_gain_control;
    float angle;
} encoder_reading_t;


bool read_angle(I2C_HandleTypeDef* ic2_handle, float* out);
bool read_full_encoder(I2C_HandleTypeDef* ic2_handle, encoder_reading_t* out);

#endif /* INC_ENCODER_READER_H_ */
