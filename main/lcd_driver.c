#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "st7735.h"

static const char* TAG = "lcd driver";

void lcd_driver_task(void *pvParameter)
{
  st7735_initr(INITR_144GREENTAB);
  st7735_fillscreen(ST7735_BLACK);

  while(1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void
lcd_driver_init(void)
{
  ESP_LOGI(TAG, "starting lcd driver");

  xTaskCreate(lcd_driver_task, "lcd_driver_task", 4096, NULL, 5, NULL);
}
