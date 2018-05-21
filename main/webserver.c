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
#include "mongoose.h"

const static char* TAG = "webserver";

static inline void
webapi_not_found(struct mg_connection* nc, struct http_message* hm)
{
  mg_printf(nc, "%s",
      "HTTP/1.1 404 Not Found\r\n"
      "Content-Length: 0\r\n\r\n");
}

static inline void
webapi_imu_status(struct mg_connection* nc, struct http_message* hm)
{
  struct imu_sensor_data_t data;

  imu_task_get_raw_values(&data);

  mg_printf(nc, "%s",
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/json\r\n"
      "Transfer-Encoding: chunked\r\n\r\n");

  mg_printf_http_chunk(nc, "{\"data\": {");

  mg_printf_http_chunk(nc, "accel_raw: [ %d,%d,%d ],",
      data.accel_raw[0],
      data.accel_raw[1],
      data.accel_raw[2]);
  mg_printf_http_chunk(nc, "accel: [%.2f, %.2f, %.2f],",
      data.accel[0],
      data.accel[1],
      data.accel[2]);

  mg_printf_http_chunk(nc, "gyro_raw: [ %d,%d,%d ],",
      data.gyro_raw[0],
      data.gyro_raw[1],
      data.gyro_raw[2]);
  mg_printf_http_chunk(nc, "gyro: [%.2f, %.2f, %.2f],",
      data.gyro[0],
      data.gyro[1],
      data.gyro[2]);

  mg_printf_http_chunk(nc, "mag_raw: [ %d,%d,%d ],",
      data.mag_raw[0],
      data.mag_raw[1],
      data.mag_raw[2]);
  mg_printf_http_chunk(nc, "mag: [%.2f, %.2f, %.2f]",
      data.mag[0],
      data.mag[1],
      data.mag[2]);

  mg_printf_http_chunk(nc, "}}");

  mg_send_http_chunk(nc, "", 0);
}

static void
mg_ev_handler(struct mg_connection* nc, int ev, void* ev_data)
{
  struct http_message* hm = (struct http_message*)ev_data;

  switch(ev)
  {
  case MG_EV_HTTP_REQUEST:
    if(mg_vcmp(&hm->method, "GET") == 0)
    {
      if(mg_vcmp(&hm->uri, "/imu/status") == 0)
      {
        webapi_imu_status(nc, hm);
      }
      else
      {
        webapi_not_found(nc, hm);
      }
    }
    else
    {
      webapi_not_found(nc, hm);
    }
    break;

  default:
    break;
  }
}


static void
webserver_task(void* pvParameters)
{
  struct mg_mgr           mgr;
  struct mg_connection*   nc;

  mg_mgr_init(&mgr, NULL);

  nc = mg_bind(&mgr, "80", mg_ev_handler);
  if(nc == NULL)
  {
    ESP_LOGE(TAG, "mg_bind failed");
  }

  mg_set_protocol_http_websocket(nc);

  ESP_LOGI(TAG, "entering main loop");

  while(1)
  {
    mg_mgr_poll(&mgr, 1000);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  mg_mgr_free(&mgr);
}

void
webserver_init(void)
{
  ESP_LOGI(TAG, "initialing webserver");

  xTaskCreate(webserver_task, "webserver_task", 4096, NULL, 5, NULL);
}
