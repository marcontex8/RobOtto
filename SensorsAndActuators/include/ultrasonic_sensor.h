/*
 * ultrasonic_sensor.h
 *
 *  Created on: Dec 28, 2025
 *      Author: marco
 */

#ifndef INCLUDE_ULTRASONIC_SENSOR_H_
#define INCLUDE_ULTRASONIC_SENSOR_H_


#include "robotto_common.h"


void triggerSensor(void);

RobottoErrorCode getMeasurementIfReady(uint32_t* time);

#endif /* INCLUDE_ULTRASONIC_SENSOR_H_ */
