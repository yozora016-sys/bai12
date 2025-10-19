#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* khai bao handle moi */
TaskHandle_t ledTaskHandle;
TaskHandle_t ctrlTaskHandle;
QueueHandle_t delayQueueHandle;

/* prototyp ham */
void led_config(void);
void led_task(void *pvParameters);
void ctrl_task(void *pvParameters);

int main(void)
{ 
    led_config();

    /* tao queue 1 phan tu, kich thuoc uint32_t */
    delayQueueHandle = xQueueCreate(1, sizeof(uint32_t));
    if (delayQueueHandle == NULL) {
        while (1) { }
    }

    /* tao task LED (prio thap) */
    if (xTaskCreate(led_task, "LED_TASK", 128, NULL, 1, &ledTaskHandle) != pdPASS) {
        while (1) { }
    }

    /* tao task CTRL (prio cao hon) */
    if (xTaskCreate(ctrl_task, "CTRL_TASK", 128, NULL, 2, &ctrlTaskHandle) != pdPASS) {
        while (1) { }
    }

    /* bat dau scheduler */
    vTaskStartScheduler();

    while (1) { }
}

/* cau hinh PA0 lam output push-pull */
void led_config(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
}

/* task LED:
    - nhan gia tri delay tu queue neu co (non-blocking)
    - doi trang thai PA0 va delay theo gia tri nhan duoc (ms)
*/
void led_task(void *pvParameters)
{
    uint32_t current_delay = 200; /* gia tri mac dinh (ms) */
    uint32_t received_delay;

    (void) pvParameters;

    while (1) {
        /* neu co gia tri moi thi cap nhat, khong cho block */
        if (xQueueReceive(delayQueueHandle, &received_delay, 0) == pdPASS) {
            current_delay = received_delay;
        }

        /* toggle PA0 */
        GPIOA->ODR ^= GPIO_Pin_0;

        /* delay theo ms chuyen sang ticks */
        vTaskDelay(pdMS_TO_TICKS(current_delay));
    }
}

/* task CTRL:
    - luan phien ghi 100 ms (nhay nhanh) va 200 ms (nhay cham)
    - su dung xQueueOverwrite de dam bao luu gia tri moi nhat */
void ctrl_task(void *pvParameters)
{
    uint32_t delay_fast = 100;  /* nhay nhanh */
    uint32_t delay_slow = 200;  /* nhay cham */
    uint8_t state = 0;

    (void) pvParameters;

    while (1) {
        if (state == 0) {
            xQueueOverwrite(delayQueueHandle, &delay_fast);
            vTaskDelay(pdMS_TO_TICKS(5000)); /* cho 5 giay */
            state = 1;
        } else {
            xQueueOverwrite(delayQueueHandle, &delay_slow);
            vTaskDelay(pdMS_TO_TICKS(7000)); /* cho 7 giay */
            state = 0;
        }
    }
}
