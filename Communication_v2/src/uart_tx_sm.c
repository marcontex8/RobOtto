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
	UART_TX_STATUS_TRANSMITTING
} UartTxState;


static UartTxState state = UART_TX_STATUS_IDLE;


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &H_UART_ESP)
    {
    	postNewCommunicationEventFromISR(EVENT_UART_TX_COMPLETE, NULL);
    }
}


UartTxState runUartTxStateIdle(CommunicationEvent event)
{
	UartTxState next_state = UART_TX_STATUS_IDLE;
	if(event.id == EVENT_UART_TX_REQUEST)
	{
		UartTxData* data = (UartTxData*)event.data;
		HAL_StatusTypeDef result = HAL_UART_Transmit_DMA(&H_UART_ESP, data->buffer, data->size);
		if (result != HAL_OK)
		{
			postNewCommunicationEvent(EVENT_UART_TX_ERROR, NULL);
			next_state = UART_TX_STATUS_IDLE;
		}
		else
		{
			next_state = UART_TX_STATUS_TRANSMITTING;
		}
	}

	return next_state;
}


UartTxState runUartTxStateTransmitting(CommunicationEvent event)
{
	UartTxState next_state = UART_TX_STATUS_TRANSMITTING;

	if(event.id == EVENT_UART_TX_COMPLETE)
	{
		next_state = UART_TX_STATUS_IDLE;
	}

	// TODO: there is probably a way to manage transmission error that could be implemented
	// else if(event->id == EVENT_UART_TX_ERROR)
	// {
	// 	next_state = UART_TX_STATUS_IDLE;
	// }

	return next_state;
}

void uart_tx_handleEvent(CommunicationEvent event)
{
	switch(state)
	{
	case UART_TX_STATUS_IDLE:
		state = runUartTxStateIdle(event);
		break;
	case UART_TX_STATUS_TRANSMITTING:
		state = runUartTxStateTransmitting(event);
		break;
	default:
		// should never happen
	}
}
