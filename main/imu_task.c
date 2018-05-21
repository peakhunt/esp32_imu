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
static imu_t                      _imu;

static void
__read_raw_imu_data(imu_t* imu)
{
  // read full raw data
  mpu9250_read_all(&imu->mpu9250, &imu->sensor_data);
}

static void
imu_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting imu task");

  memset(&_imu, 0, sizeof(_imu));

  mpu9250_init(&_imu.mpu9250, MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s);

  while(1)
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);

    __read_raw_imu_data(&_imu);

    xSemaphoreGive(_mutex);

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
imu_task_get_raw_values(struct imu_sensor_data_t* data)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  memcpy(data, &_imu.sensor_data, sizeof(struct imu_sensor_data_t));

  xSemaphoreGive(_mutex);
}
