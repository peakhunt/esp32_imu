#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define BLINK_GPIO CONFIG_BLINK_GPIO
#define BLINK_DELAY 100

static const char* TAG = "blinky";

void blink_task(void *pvParameter)
{
  gpio_pad_select_gpio(BLINK_GPIO);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

  while(1)
  {
    /* Blink off (output low) */
    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);

    /* Blink on (output high) */
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(BLINK_DELAY / portTICK_PERIOD_MS);
  }
}

void
blinky_init(void)
{
  ESP_LOGI(TAG, "starting blinky");
  //
  // XXX
  // with this task stack size. calling ESP_LOGI will overflow the stack.
  // but at the same time, does the blinky task deserves a 1K task stack?
  // I don't think so.
  //
  xTaskCreate(blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
