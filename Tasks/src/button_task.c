#include "button_task.h"
#include "robotto_common.h"
#include "FreeRTOS.h"
#include "queue.h"


#include "stm32f4xx.h"
#include "stm32f4xx_hal_gpio.h"

extern QueueHandle_t behavior_queue_handle;
extern TaskHandle_t buttonTaskHandle;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        static TickType_t last = 0;
        TickType_t now = xTaskGetTickCountFromISR();

        if ((now - last) > pdMS_TO_TICKS(50))
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            vTaskNotifyGiveFromISR(buttonTaskHandle, &xHigherPriorityTaskWoken);

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }

        last = now;
    }
}

void buttonTask(void *argument)
{
    for (;;)
    {
        static RobottoBehavior behavior = ROBOTTO_BEHAVIOR_IDLE;
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ROBOTTO_BEHAVIOR_IDLE == behavior)
        {
            behavior = ROBOTTO_BEHAVIOR_RUNNING;
        }
        else  // RUNNING
        {
            behavior = ROBOTTO_BEHAVIOR_IDLE;
        }
        xQueueOverwrite(behavior_queue_handle, &behavior);
    }
}