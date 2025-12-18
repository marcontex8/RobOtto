/*
 * network_communication.h
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#ifndef INCLUDE_NETWORK_COMMUNICATION_H_
#define INCLUDE_NETWORK_COMMUNICATION_H_


typedef enum{
	NET_INIT_IDLE,
	NET_INIT_WAITING_ESP_INIT,
	NET_INIT_STEPS_PROGRESS,
	NET_INIT_SUCCESS,
	NET_INIT_ERROR,
} NetworkInitializationStatus;



NetworkInitializationStatus initNetworkCommunication();

void updateIncomingData();

const char* getError();

#endif /* INCLUDE_NETWORK_COMMUNICATION_H_ */
