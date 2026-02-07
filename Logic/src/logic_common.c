/*
 * logic_common.c
 *
 *  Created on: Feb 7, 2026
 *      Author: marco
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

