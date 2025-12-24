/*
 * uart_writer.c
 *
 *  Created on: Dec 5, 2025
 *      Author: marco
 */

#include <stdbool.h>
#include <string.h>


#include "stm32f4xx.h"
#include "stm32f4xx_hal_uart.h"

#include "FreeRTOS.h"
#include "task.h"

#include "uart_writer.h"

extern UART_HandleTypeDef huart4;
#define H_UART_ESP huart4
#define ESP_UART_DMA_TX_BUF_SIZE 1024
uint8_t esp_uart_dma_tx_buf[ESP_UART_DMA_TX_BUF_SIZE];

static bool complete = true;



RobottoErrorCode SendMessage(const char* message)
{
	if (false == complete || NULL == message)
	{
		return ROBOTTO_ERROR;
	}

	const unsigned int size = strlen(message);
	if(size+2 > ESP_UART_DMA_TX_BUF_SIZE)
	{
		return ROBOTTO_ERROR;
	}

	memcpy(esp_uart_dma_tx_buf, (const uint8_t*)message, size);

	esp_uart_dma_tx_buf[size] = '\r';
	esp_uart_dma_tx_buf[size+1] = '\n';

	complete = false;

	HAL_UART_Transmit_DMA(&H_UART_ESP, esp_uart_dma_tx_buf, size+2);
	return ROBOTTO_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart != &H_UART_ESP)
    {
    	return;
    }

    complete = true;
}
