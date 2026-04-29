/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef COMMUNICATION_INCLUDE_COMMUNICATION_H
#define COMMUNICATION_INCLUDE_COMMUNICATION_H

#include "robotto_common.h"

RobottoErrorCode Communication_initializeCommunication();

void Communication_task(void * argument);

#endif /* COMMUNICATION_INCLUDE_COMMUNICATION_H */
