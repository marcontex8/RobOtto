/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef SENSORSANDACTUATORS_INCLUDE_ENCODER_READER_H
#define SENSORSANDACTUATORS_INCLUDE_ENCODER_READER_H

#include <stdbool.h>

#include "robotto_common.h"
#include "stm32f4xx_hal.h"

#include "sensors_and_actuators_common.h"

typedef struct
{
    uint8_t magnet_detected;
    uint8_t too_strong;
    uint8_t too_weak;
    float automatic_gain_control;
    float angle;
} EncoderStatus;

RobottoErrorCode readAngleRad(Wheel wheel, float *out);
RobottoErrorCode readFullEncoder(Wheel wheel, EncoderStatus *out);

#endif /* SENSORSANDACTUATORS_INCLUDE_ENCODER_READER_H */
