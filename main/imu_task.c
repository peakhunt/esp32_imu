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

static imu_t        _imu;
static mpu9250_t    _mpu9250;

static void
imu_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting imu task");


  mpu9250_init(&_mpu9250, MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s, &_imu.lsb);

  while(1)
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);

    mpu9250_read_all(&_mpu9250, &_imu.raw);
    imu_update(&_imu);

    xSemaphoreGive(_mutex);

    vTaskDelay(IMU_POLL_INTERVAL / portTICK_PERIOD_MS);
  }
}

void
imu_task_init(void)
{
  ESP_LOGI(TAG, "initialing IMU task");

  imu_init(&_imu);

  _mutex = xSemaphoreCreateMutex();

  xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);
}

void
imu_task_get_sensor_data(imu_sensor_data_t* data)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  // memcpy(data, &_imu.sensor_data, sizeof(imu_sensor_data_t));

  xSemaphoreGive(_mutex);
}

void
imu_task_get_raw_and_data(imu_sensor_data_t* raw, imu_data_t* data)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  memcpy(raw, &_imu.adjusted, sizeof(imu_sensor_data_t));
  memcpy(data, &_imu.data, sizeof(imu_data_t));

  xSemaphoreGive(_mutex);
}
