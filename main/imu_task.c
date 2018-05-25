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
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "imu_task.h"
#include "mpu9250.h"

#include "sdkconfig.h"

#define IMU_POLL_INTERVAL               5

const static char* TAG = "imu_task";

typedef enum
{
  imu_task_perform_mag_calibration,
  imu_task_perform_gyro_calibration,
  imu_task_perform_accel_calibration,
} imu_task_command_t;

static SemaphoreHandle_t          _mutex;
static imu_t        _imu;
static mpu9250_t    _mpu9250;
static volatile uint32_t          _loop_cnt = 0;
static xQueueHandle _cmd_queue = NULL;

static void
imu_task(void* pvParameters)
{
  const TickType_t xDelay = IMU_POLL_INTERVAL / portTICK_PERIOD_MS;
  uint32_t cmd;
  struct timeval    cal_start_time,
                    now;

  ESP_LOGI(TAG, "starting imu task");


  mpu9250_init(&_mpu9250, MPU9250_Accelerometer_8G, MPU9250_Gyroscope_500s, &_imu.lsb);

  while(1)
  {
    if(xQueueReceive(_cmd_queue, &cmd, xDelay))
    {
      switch(cmd)
      {
      case imu_task_perform_mag_calibration:
        gettimeofday(&cal_start_time, NULL);
        imu_start_mag_calibration(&_imu);
        break;

      case imu_task_perform_gyro_calibration:
        gettimeofday(&cal_start_time, NULL);
        imu_start_gyro_calibration(&_imu);
        break;

      case imu_task_perform_accel_calibration:
        gettimeofday(&cal_start_time, NULL);
        imu_start_accel_calibration(&_imu);
        break;
      }
    }

    xSemaphoreTake(_mutex, portMAX_DELAY);

    mpu9250_read_all(&_mpu9250, &_imu.raw);
    imu_update(&_imu);

    if(_imu.mode != imu_mode_normal)
    {
      gettimeofday(&now, NULL);
      
      switch(_imu.mode)
      {
      case imu_mode_mag_calibrating:
        if((now.tv_sec - cal_start_time.tv_sec) >= 30)
        {
          imu_finish_mag_calibration(&_imu);
        }
        break;

      case imu_mode_gyro_calibrating:
        if((now.tv_sec - cal_start_time.tv_sec) >= 30)
        {
          imu_finish_gyro_calibration(&_imu);
        }
        break;

      case imu_mode_accel_calibrating:
        if((now.tv_sec - cal_start_time.tv_sec) >= 120)
        {
          imu_finish_accel_calibration(&_imu);
        }
        break;

      default:
        break;
      }
    }
    xSemaphoreGive(_mutex);
    _loop_cnt++;
  }
}

void
imu_task_init(void)
{
  ESP_LOGI(TAG, "initialing IMU task");

  imu_init(&_imu);

  _mutex = xSemaphoreCreateMutex();

  _cmd_queue = xQueueCreate(10, sizeof(uint32_t));

  xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);
}

void
imu_task_get_raw_and_data(imu_mode_t* mode, imu_sensor_data_t* raw, imu_data_t* data)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  *mode = _imu.mode;
  memcpy(raw, &_imu.adjusted, sizeof(imu_sensor_data_t));
  memcpy(data, &_imu.data, sizeof(imu_data_t));

  xSemaphoreGive(_mutex);
}

uint32_t
imu_task_get_loop_cnt(void)
{
  return _loop_cnt;
}

void
imu_task_do_mag_calibration(void)
{
  imu_task_command_t  cmd = imu_task_perform_mag_calibration;
  xQueueSend(_cmd_queue, &cmd, 1000 / portTICK_PERIOD_MS);
}
