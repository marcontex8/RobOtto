#ifndef COMMUNICATION_SM_HANDLERS_H_
#define COMMUNICATION_SM_HANDLERS_H_

#include "communication_events.h"

void at_handleEvent(const CommunicationEvent* event);
void connection_handleEvent(const CommunicationEvent* event);
void uart_rx_handleEvent(const CommunicationEvent* event);
void uart_tx_handleEvent(const CommunicationEvent* event);
void telemetry_handleEvent(const CommunicationEvent* event);


#endif /* COMMUNICATION_SM_HANDLERS_H_ */