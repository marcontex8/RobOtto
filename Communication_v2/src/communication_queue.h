#ifndef COMMUNICATION_QUEUE_H_
#define COMMUNICATION_QUEUE_H_

#include "robotto_common.h"
#include "communication_events.h"

RobottoErrorCode initializeQueue();

void postNewCommunicationEvent(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventWithNoData(CommunicationEventId event_id);
void postNewCommunicationEventFromISR(CommunicationEventId event_id, CommunicationEventData data);
void postNewCommunicationEventFromISRWithNoData(CommunicationEventId event_id);

void triggerCommunicationInitialization();

bool getNextCommunicationEvent(CommunicationEvent* event);

#endif /* COMMUNICATION_QUEUE_H_ */