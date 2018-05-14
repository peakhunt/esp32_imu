#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "ST7735.h"

static const char* TAG = "lcd driver";

void lcd_driver_task(void *pvParameter)
{
  ST7735_InitR(INITR_REDTAB);
  // ST7735_InitB();
  // ST7735_SetRotation(3);

  ST7735_FillScreen(ST7735_BLACK);

  // Output_Off();

  while(1)
  {
    //Output_On();
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
    //Output_Off();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void
lcd_driver_init(void)
{
  ESP_LOGI(TAG, "starting lcd driver");

  xTaskCreate(lcd_driver_task, "lcd_driver_task", 4096, NULL, 5, NULL);
}
