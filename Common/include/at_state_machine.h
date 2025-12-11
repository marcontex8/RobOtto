/*
 * at_state_machine.h
 *
 *  Created on: Dec 5, 2025
 *      Author: marco
 */

#ifndef INCLUDE_AT_STATE_MACHINE_H_
#define INCLUDE_AT_STATE_MACHINE_H_


#include "robotto_common.h"


typedef enum{
	AT_STATUS_IDLE,
	AT_STATUS_WAITING,
	AT_STATUS_DONE,
} ATStatus;

typedef enum{
	AT_ANSWER_ERROR,
	AT_ANSWER_OK,
} ATAnswer;


ATStatus ATSM_runRequest(const char* command, ATAnswer* answer);

void ATSM_processNewData();

void ATSM_newRecievedData(const uint8_t *data, uint16_t len);



#endif /* INCLUDE_AT_STATE_MACHINE_H_ */
