/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */

#ifndef COMMUNICATION_SRC_COMMUNICATION_SM_HANDLERS_H
#define COMMUNICATION_SRC_COMMUNICATION_SM_HANDLERS_H

#include "communication_events.h"

void at_handleEvent(const CommunicationEvent *event);
void connection_handleEvent(const CommunicationEvent *event);
void uart_rx_handleEvent(const CommunicationEvent *event);
void uart_tx_handleEvent(const CommunicationEvent *event);
void telemetry_handleEvent(const CommunicationEvent *event);

#endif /* COMMUNICATION_SRC_COMMUNICATION_SM_HANDLERS_H */
