/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#include "communication.h"
#include "communication_queue.h"

RobottoErrorCode Communication_initializeCommunication()
{
    return initializeQueue();
}
