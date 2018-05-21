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
#include "freertos/semphr.h"

#include "imu_task.h"
#include "mpu9250.h"

#include "sdkconfig.h"

#define IMU_POLL_INTERVAL             30

const static char* TAG = "imu_task";

static SemaphoreHandle_t          _mutex;
static imu_t                      _imu_data;

static void
__read_raw_imu_data(imu_t* imu)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  // read full raw data
  if(mpu9250_read_all(imu->accel_raw, imu->gyro_raw, &imu->temp_raw) == FALSE)
  {
    return;
  }

  xSemaphoreGive(_mutex);
}

static void
imu_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting imu task");

  memset(&_imu_data, 0, sizeof(_imu_data));
  mpu9250_init(MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s);

  while(1)
  {
    __read_raw_imu_data(&_imu_data);

    vTaskDelay(IMU_POLL_INTERVAL / portTICK_PERIOD_MS);
  }
}

void
imu_task_init(void)
{
  ESP_LOGI(TAG, "initialing IMU task");

  _mutex = xSemaphoreCreateMutex();

  xTaskCreate(&imu_task, "imu_task", 4096, NULL, 5, NULL);
}

void
imu_task_get_raw_values(int16_t a[3], int16_t g[3], int16_t* temp)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  a[0] = _imu_data.accel_raw[0];
  a[1] = _imu_data.accel_raw[1];
  a[2] = _imu_data.accel_raw[2];

  g[0] = _imu_data.gyro_raw[0];
  g[1] = _imu_data.gyro_raw[1];
  g[2] = _imu_data.gyro_raw[2];

  *temp = _imu_data.temp_raw;

  xSemaphoreGive(_mutex);
}
