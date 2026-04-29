#include "communication.h"
#include "communication_queue.h"

RobottoErrorCode Communication_initializeCommunication()
{
    return initializeQueue();
}