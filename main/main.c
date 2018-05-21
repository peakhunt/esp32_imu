#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

#include "blinky.h"
#include "app_wifi.h"
#include "imu_task.h"
#include "shell.h"
#include "lcd_driver.h"

void app_main()
{
  esp_err_t ret = nvs_flash_init();

  if(ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  app_wifi_init();
  blinky_init();
  imu_task_init();
  lcd_driver_init();
  shell_init();
}
