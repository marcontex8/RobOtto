/*
 * uart_reader.c
 *
 *  Created on: Dec 1, 2025
 *      Author: marco
 */

#include <string.h>


#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "uart_reader.h"
#include "robotto_common.h"
#include "at_state_machine.h"


#define H_UART_ESP huart4
#define ESP_UART_DMA_RX_BUF_SIZE 1024

extern UART_HandleTypeDef huart4;
extern TaskHandle_t communication_manager_handles;

uint8_t esp_uart_dma_rx_buf[ESP_UART_DMA_RX_BUF_SIZE];


void UART4_IRQHandler()
{
    HAL_UART_IRQHandler(&H_UART_ESP);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart != &H_UART_ESP)
	{
		return;
	}

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (communication_manager_handles != NULL)
    {
       vTaskNotifyGiveFromISR(communication_manager_handles, &xHigherPriorityTaskWoken);
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void ESP_UART_RxInit()
{
	HAL_UARTEx_ReceiveToIdle_DMA(&H_UART_ESP, esp_uart_dma_rx_buf, ESP_UART_DMA_RX_BUF_SIZE);
}


void ESP_UART_fetchAndParseNewData()
{
	static uint16_t tail_ptr = 0;
	uint16_t head_ptr = ESP_UART_DMA_RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(H_UART_ESP.hdmarx);

	if (head_ptr == tail_ptr)
	{
		SEGGER_SYSVIEW_Print("No new data");
		return;
	}

	SEGGER_SYSVIEW_Print("New data!");
	if (head_ptr > tail_ptr)
	{
		uint16_t new_data_length = head_ptr - tail_ptr;
		ATSM_newRecievedData(&esp_uart_dma_rx_buf[tail_ptr], new_data_length);
	}
	else
	{
		uint16_t len1 = ESP_UART_DMA_RX_BUF_SIZE - tail_ptr;
		ATSM_newRecievedData(&esp_uart_dma_rx_buf[tail_ptr], len1);

	    uint16_t len2 = head_ptr;
	    ATSM_newRecievedData(&esp_uart_dma_rx_buf[0], len2);
	}

	ATSM_processNewData();

	tail_ptr = head_ptr;
}
