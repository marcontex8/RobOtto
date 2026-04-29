#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "robotto_common.h"

RobottoErrorCode Communication_initializeCommunication();

void Communication_task(void * argument);

#endif /* COMMUNICATION_H_ */
