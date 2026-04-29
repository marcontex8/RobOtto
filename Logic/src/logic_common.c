/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */
#include "logic_common.h"
#include <math.h>


float wrapToPI(float angle)
{
    while (angle > (float)M_PI) {
        angle -= 2.0f * (float)M_PI;
    }
    while (angle < -(float)M_PI) {
        angle += 2.0f * (float)M_PI;
    }
    return angle;
}

