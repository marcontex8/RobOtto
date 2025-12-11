/*
 * network_communication.h
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#ifndef INCLUDE_NETWORK_COMMUNICATION_H_
#define INCLUDE_NETWORK_COMMUNICATION_H_


typedef enum{
	COMM_STATUS_IN_PROGRESS,
	COMM_STATUS_DONE,
	COMM_STATUS_ERROR,
} CommunicationStatus;



CommunicationStatus initNetworkCommunication();

void parseNewData();

#endif /* INCLUDE_NETWORK_COMMUNICATION_H_ */
