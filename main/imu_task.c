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
__convert_raw_to_eng(imu_t* imu)
{
  // FIXME
  // accel/gyro conversion

  imu->temp = (imu->temp_raw / 340333.87f + 21.0f);
}

static void
__read_raw_imu_data(imu_t* imu)
{
  // read full raw data
  mpu9250_read_all(imu->accel_raw, imu->gyro_raw, &imu->temp_raw);
}

static void
imu_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting imu task");

  memset(&_imu_data, 0, sizeof(_imu_data));
  mpu9250_init(MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s);

  while(1)
  {
    xSemaphoreTake(_mutex, portMAX_DELAY);

    __read_raw_imu_data(&_imu_data);
    __convert_raw_to_eng(&_imu_data);

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
imu_task_get_raw_values(imu_t* imu)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  memcpy(imu, &_imu_data, sizeof(imu_t));

  xSemaphoreGive(_mutex);
}
