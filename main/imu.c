#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "list.h"
#include "shell.h"
#include "io_driver.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mpu9250.h"

#include "sdkconfig.h"


const static char* TAG = "imu";

static void
imu_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting imu task");

  mpu9250_init(MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s);

  while(1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void
imu_init(void)
{
  ESP_LOGI(TAG, "initialing IMU");

  xTaskCreate(&imu_task, "imu_task", 4096, NULL, 5, NULL);
}
