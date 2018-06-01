#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "generic_list.h"
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

#define IMU_POLL_INTERVAL               2

const static char* TAG = "imu_task";

typedef enum
{
  imu_task_perform_mag_calibration,
  imu_task_perform_gyro_calibration,
  imu_task_perform_accel_calibration_init,
  imu_task_perform_accel_calibration_step,
  imu_task_perform_accel_calibration_finish,
} imu_task_command_t;

static SemaphoreHandle_t          _mutex;
static imu_t        _imu;
static mpu9250_t    _mpu9250;
static volatile uint32_t          _loop_cnt = 0;
static xQueueHandle _cmd_queue = NULL;

static void
imu_task_load_calibration(void)
{
  esp_err_t   err;
  nvs_handle  nvs_handle;
  static const imu_sensor_calib_data_t    cal_default = 
  {
    .accel_off  = { 0, 0, 0},
    .accel_scale = { 4096, 4096, 4096 },
    .gyro_off = {0, 0, 0},
    .mag_bias = {0, 0, 0},
    .mag_declination = 0.0f
  };

  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if(err != ESP_OK)
  {
    memcpy(&_imu.cal, &cal_default, sizeof(imu_sensor_calib_data_t));
    ESP_LOGI(TAG, "nvs_open failed. going with all defaults");
    return;
  }

  if(nvs_get_i16(nvs_handle, "accel_off_x", &_imu.cal.accel_off[0]) != ESP_OK)
  {
    _imu.cal.accel_off[0] = cal_default.accel_off[0];
  }

  if(nvs_get_i16(nvs_handle, "accel_off_y", &_imu.cal.accel_off[1]) != ESP_OK)
  {
    _imu.cal.accel_off[1] = cal_default.accel_off[1];
  }

  if(nvs_get_i16(nvs_handle, "accel_off_z", &_imu.cal.accel_off[2]) != ESP_OK)
  {
    _imu.cal.accel_off[2] = cal_default.accel_off[2];
  }

  if(nvs_get_i16(nvs_handle, "accel_scale_x", &_imu.cal.accel_scale[0]) != ESP_OK)
  {
    _imu.cal.accel_scale[0] = cal_default.accel_scale[0];
  }

  if(nvs_get_i16(nvs_handle, "accel_scale_y", &_imu.cal.accel_scale[1]) != ESP_OK)
  {
    _imu.cal.accel_scale[1] = cal_default.accel_scale[1];
  }

  if(nvs_get_i16(nvs_handle, "accel_scale_z", &_imu.cal.accel_scale[2]) != ESP_OK)
  {
    _imu.cal.accel_scale[2] = cal_default.accel_scale[2];
  }

  if(nvs_get_i16(nvs_handle, "gyro_off_x", &_imu.cal.gyro_off[0]) != ESP_OK)
  {
    _imu.cal.gyro_off[0] = cal_default.gyro_off[0];
  }

  if(nvs_get_i16(nvs_handle, "gyro_off_y", &_imu.cal.gyro_off[1]) != ESP_OK)
  {
    _imu.cal.gyro_off[1] = cal_default.gyro_off[1];
  }

  if(nvs_get_i16(nvs_handle, "gyro_off_z", &_imu.cal.gyro_off[2]) != ESP_OK)
  {
    _imu.cal.gyro_off[2] = cal_default.gyro_off[2];
  }

  if(nvs_get_i16(nvs_handle, "mag_bias_x", &_imu.cal.mag_bias[0]) != ESP_OK)
  {
    _imu.cal.mag_bias[0] = cal_default.mag_bias[0];
  }

  if(nvs_get_i16(nvs_handle, "mag_bias_y", &_imu.cal.mag_bias[1]) != ESP_OK)
  {
    _imu.cal.mag_bias[1] = cal_default.mag_bias[1];
  }

  if(nvs_get_i16(nvs_handle, "mag_bias_z", &_imu.cal.mag_bias[2]) != ESP_OK)
  {
    _imu.cal.mag_bias[2] = cal_default.mag_bias[2];
  }

  nvs_close(nvs_handle);
}

static void
imu_task_save_calibration(void)
{
  esp_err_t   err;
  nvs_handle  nvs_handle;
  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if(err != ESP_OK)
  {
    ESP_LOGE(TAG, "nvs_open failed. saving calibration data failed");
  }

  nvs_set_i16(nvs_handle, "accel_off_x", _imu.cal.accel_off[0]);
  nvs_set_i16(nvs_handle, "accel_off_y", _imu.cal.accel_off[1]);
  nvs_set_i16(nvs_handle, "accel_off_z", _imu.cal.accel_off[2]);
  nvs_set_i16(nvs_handle, "accel_scale_x", _imu.cal.accel_scale[0]);
  nvs_set_i16(nvs_handle, "accel_scale_y", _imu.cal.accel_scale[1]);
  nvs_set_i16(nvs_handle, "accel_scale_z", _imu.cal.accel_scale[2]);
  nvs_set_i16(nvs_handle, "gyro_off_x", _imu.cal.gyro_off[0]);
  nvs_set_i16(nvs_handle, "gyro_off_y", _imu.cal.gyro_off[1]);
  nvs_set_i16(nvs_handle, "gyro_off_z", _imu.cal.gyro_off[2]);
  nvs_set_i16(nvs_handle, "mag_bias_x", _imu.cal.mag_bias[0]);
  nvs_set_i16(nvs_handle, "mag_bias_y", _imu.cal.mag_bias[1]);
  nvs_set_i16(nvs_handle, "mag_bias_z", _imu.cal.mag_bias[2]);

  nvs_commit(nvs_handle);
  nvs_close(nvs_handle);
}

static void
imu_task(void* pvParameters)
{
  const TickType_t xDelay = IMU_POLL_INTERVAL / portTICK_PERIOD_MS;
  uint32_t cmd;
  struct timeval    cal_start_time,
                    now;

  ESP_LOGI(TAG, "starting imu task");


  mpu9250_init(&_mpu9250, MPU9250_Accelerometer_8G, MPU9250_Gyroscope_1000s, &_imu.lsb);

  while(1)
  {
    if(xQueueReceive(_cmd_queue, &cmd, xDelay))
    {
      switch(cmd)
      {
      case imu_task_perform_mag_calibration:
        gettimeofday(&cal_start_time, NULL);
        imu_mag_calibration_start(&_imu);
        break;

      case imu_task_perform_gyro_calibration:
        gettimeofday(&cal_start_time, NULL);
        imu_gyro_calibration_start(&_imu);
        break;

      case imu_task_perform_accel_calibration_init:
        imu_accel_calibration_init(&_imu);
        break;

      case imu_task_perform_accel_calibration_step:
        gettimeofday(&cal_start_time, NULL);
        imu_accel_calibration_step_start(&_imu);
        break;

      case imu_task_perform_accel_calibration_finish:
        imu_accel_calibration_finish(&_imu);
        imu_task_save_calibration();
        break;

      default:
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
          imu_mag_calibration_finish(&_imu);
          imu_task_save_calibration();
        }
        break;

      case imu_mode_gyro_calibrating:
        if((now.tv_sec - cal_start_time.tv_sec) >= 30)
        {
          imu_gyro_calibration_finish(&_imu);
          imu_task_save_calibration();
        }
        break;

      case imu_mode_accel_calibrating:
        if((now.tv_sec - cal_start_time.tv_sec) >= 20)
        {
          imu_accel_calibration_step_stop(&_imu);
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
  imu_task_load_calibration();

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

void
imu_task_get_cal_state(imu_sensor_calib_data_t* cal)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);

  memcpy(cal, &_imu.cal, sizeof(imu_sensor_calib_data_t));

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

void
imu_task_do_gyro_calibration(void)
{
  imu_task_command_t  cmd = imu_task_perform_gyro_calibration;

  xQueueSend(_cmd_queue, &cmd, 1000 / portTICK_PERIOD_MS);
}

void
imu_task_do_accel_calibration_init(void)
{
  imu_task_command_t  cmd = imu_task_perform_accel_calibration_init;

  xQueueSend(_cmd_queue, &cmd, 1000 / portTICK_PERIOD_MS);
}

void
imu_task_do_accel_calibration_start(void)
{
  imu_task_command_t  cmd = imu_task_perform_accel_calibration_step;

  xQueueSend(_cmd_queue, &cmd, 1000 / portTICK_PERIOD_MS);
}

void
imu_task_do_accel_calibration_finish(void)
{
  imu_task_command_t  cmd = imu_task_perform_accel_calibration_finish;

  xQueueSend(_cmd_queue, &cmd, 1000 / portTICK_PERIOD_MS);
}
