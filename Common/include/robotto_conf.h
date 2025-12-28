/*
 * roboto_conf.h
 *
 *  Created on: Nov 24, 2025
 *      Author: marco
 */

#ifndef INCLUDE_ROBOTTO_CONF_H_
#define INCLUDE_ROBOTTO_CONF_H_

#include "secrets.h"


/************ PHYSICAL PARAMETERS *************/

// wheel radius [m]
#define WHEELS_RADIUS 0.0334

// maximum angular wheel speed achievable without load [rad/s]
#define MAX_ANGULAR_WHEEL_SPEED 20.7

// distance between the wheels (width)
#define WHEELS_DISTANCE 0.186


/************** NETWORK PARAMETERS **************/

// provided by "secrets.h"
#define WIFI_SSID SECRET_WIFI_SSID
#define WIFI_PWD SECRET_WIFI_PWD
#define MQTT_BROKER_IP SECRET_MQTT_BROKER_IP
#define MQTT_BROKER_PORT SECRET_MQTT_BROKER_PORT

// time to wait before starting to send AT commands [s]
#define NETWORK_RUN_DELAY_AT_STARTUP_S 10

// maximum time allowed for a network request through ESP [s]
#define NETWORK_REQUEST_TIMEOUT_S 30

// maximum amount of times that network request will be repeated
#define NETWORK_COMMANDS_MAX_REPETITIONS 6

// delay between two successive at command during init
#define NETWORK_COMMANDS_DELAY_S 5


#endif /* INCLUDE_ROBOTTO_CONF_H_ */
