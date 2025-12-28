/*
 * uart_tx_sm.c
 *
 *  Created on: Dec 18, 2025
 *      Author: marco
 */
#include <stdint.h>
#include <stddef.h>

#include <string.h>

#include "communication_events.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

extern UART_HandleTypeDef huart4;
#define H_UART_ESP huart4


typedef enum{
	UART_TX_STATUS_IDLE,
	UART_TX_STATUS_TRANSMITTING,

	UART_TX_STATUS_COUNT,
} UartTxState;


static UartTxState state = UART_TX_STATUS_IDLE;


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &H_UART_ESP)
    {
    	postNewCommunicationEventFromISRWithNoData(EVENT_UART_TX_COMPLETE);
    }
}


UartTxState onUartTxRequest(const CommunicationEventData* data)
{
	UartTxState next_state = UART_TX_STATUS_IDLE;

	HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(&H_UART_ESP, data->uart_tx.buffer, data->uart_tx.size);
	if (result != HAL_OK)
	{
		postNewCommunicationEventWithNoData(EVENT_UART_TX_ERROR);
		next_state = UART_TX_STATUS_IDLE;
	}
	else
	{
		next_state = UART_TX_STATUS_TRANSMITTING;
	}
	return next_state;
}


UartTxState onUartTxComplete(const CommunicationEventData*)
{
	return UART_TX_STATUS_IDLE;
}


typedef UartTxState (*UartTxTransitionFunction)(const CommunicationEventData* data);

static const UartTxTransitionFunction uart_tx_state_transition_table[UART_TX_STATUS_COUNT][EVENT_COUNT] = {
    [UART_TX_STATUS_IDLE] = {
        [EVENT_UART_TX_REQUEST] = onUartTxRequest,
    },
    [UART_TX_STATUS_TRANSMITTING] = {
        [EVENT_UART_TX_COMPLETE] = onUartTxComplete,
    },
};


void uart_tx_handleEvent(const CommunicationEvent* event)
{
	UartTxTransitionFunction function = uart_tx_state_transition_table[state][event->id];
	if(function != NULL)
	{
		state = function(&(event->data));
	}
}

