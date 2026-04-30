#include "robotto_setup.h"
#include "robotto_shared_queues.h"
#include "robotto_tasks.h"
#include "communication.h"
#include "i2c_busses.h"

RobottoErrorCode setupRobotto()
{
    initializeI2CMutexes();

    if (ROBOTTO_OK != createSharedQueues())
    {
        return ROBOTTO_ERROR;
    }

    if (ROBOTTO_OK != Communication_initializeCommunication())
    {
        return ROBOTTO_ERROR;
    }

    if (ROBOTTO_OK != startTasks())
    {
        return ROBOTTO_ERROR;
    }

    return ROBOTTO_OK;
}
