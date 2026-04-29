/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef COMMUNICATION_SRC_COMMUNICATION_QUEUE_H
#define COMMUNICATION_SRC_COMMUNICATION_QUEUE_H

#include "communication_events.h"
#include "robotto_common.h"

RobottoErrorCode initializeQueue();

void postNewCommunicationEvent(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventWithNoData(CommunicationEventId event_id);
void postNewCommunicationEventFromISR(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventFromISRWithNoData(CommunicationEventId event_id);

void triggerCommunicationInitialization();

bool getNextCommunicationEvent(CommunicationEvent *event);

#endif /* COMMUNICATION_SRC_COMMUNICATION_QUEUE_H */
