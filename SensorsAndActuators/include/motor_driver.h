/*
 * motor_driver.h
 *
 *  Created on: Nov 16, 2025
 *      Author: marco
 */

#ifndef SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H_
#define SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H_


#include "sensors_and_actuators_common.h"


void initializeMotorDriver();

void setMotorDutyCycle(Wheel wheel, float normalized_duty_cycle);

#endif /* SENSORSANDACTUATORS_INCLUDE_MOTOR_DRIVER_H_ */
