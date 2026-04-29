/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef LOGIC_INCLUDE_WHEEL_STATUS_ESTIMATOR_H
#define LOGIC_INCLUDE_WHEEL_STATUS_ESTIMATOR_H

#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>


typedef struct{
    float last_angle;
    float last_speed;
    float filtered_speed;
    float delta_angle;
    TickType_t last_tick;
    bool initialized;
} WheelStatus;




bool updateWheelStatusEstimation(WheelStatus* wheel_status, float angle, TickType_t tick);


#endif /* LOGIC_INCLUDE_WHEEL_STATUS_ESTIMATOR_H */
