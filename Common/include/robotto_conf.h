/*
 * roboto_conf.h
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */

#ifndef INCLUDE_ROBOTTO_CONF_H_
#define INCLUDE_ROBOTTO_CONF_H_

#include "secrets.h"

// wheel radius [m]
#define WHEELS_RADIUS 0.0334

// maximum angular wheel speed achievable without load [rad/s]
#define MAX_ANGULAR_WHEEL_SPEED 20.7

// distance between the wheels (width)
#define WHEELS_DISTANCE 0.186

#define WIFI_SSID SECRET_WIFI_SSID
#define WIFI_PWD SECRET_WIFI_PWD

#endif /* INCLUDE_ROBOTTO_CONF_H_ */
