#include <stdio.h>
#include "list.h"
#include "shell.h"
#include "io_driver.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "cli.h"

static const char* TAG   = "shell";

static io_driver_t      _io_driver;

io_driver_t*
cli_io_driver(void)
{
  return &_io_driver;
}

static void
shell_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting shell task");

  io_driver_init(&_io_driver);

  cli_init(NULL, 0);

  while(1)
  {
    io_driver_run(&_io_driver);
  }
}

void
shell_init(void)
{
  ESP_LOGI(TAG, "initializing shell");

  xTaskCreate(&shell_task, "shell_task", 4096, NULL, 5, NULL);
}
