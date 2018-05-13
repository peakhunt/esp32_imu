#include <stdio.h>
#include <sys/time.h>
#include "list.h"
#include "shell.h"
#include "io_driver.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "cli.h"

#include "esp_system.h"
#include "esp_spi_flash.h"

////////////////////////////////////////////////////////////////////////////////
//
// private prototypes
//
////////////////////////////////////////////////////////////////////////////////
static void cli_command_sysinfo(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_systime(cli_intf_t* intf, int argc, const char** argv);

static const char* TAG   = "shell";

static io_driver_t      _io_driver;


static cli_command_t    _app_commands[] =
{
  {
    "sysinfo",
    "show system info",
    cli_command_sysinfo,
  },
  {
    "systime",
    "show system uptime",
    cli_command_systime,
  }
};


////////////////////////////////////////////////////////////////////////////////
//
// application specific CLI commands implementation
//
////////////////////////////////////////////////////////////////////////////////
  static void
cli_command_sysinfo(cli_intf_t* intf, int argc, const char** argv)
{
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);

  cli_printf(intf, CLI_EOL);
  cli_printf(intf, "This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
      chip_info.cores,
      (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
      (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  cli_printf(intf, "silicon revision %d, ", chip_info.revision);
  cli_printf(intf, "%dMB %s flash"CLI_EOL, spi_flash_get_chip_size() / (1024 * 1024),
      (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

  static void
cli_command_systime(cli_intf_t* intf, int argc, const char** argv)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  cli_printf(intf, CLI_EOL);
  cli_printf(intf, "uptime: %d"CLI_EOL, tv.tv_sec);
}


////////////////////////////////////////////////////////////////////////////////
//
// global singleton for CLI io driver
//
////////////////////////////////////////////////////////////////////////////////
io_driver_t*
cli_io_driver(void)
{
  return &_io_driver;
}

////////////////////////////////////////////////////////////////////////////////
//
// shell task. a simple I/O driver.
//
////////////////////////////////////////////////////////////////////////////////
static void
shell_task(void* pvParameters)
{
  ESP_LOGI(TAG, "starting shell task");

  io_driver_init(&_io_driver);

  cli_init(_app_commands, NARRAY(_app_commands), CONFIG_TELNET_PORT);

  while(1)
  {
    io_driver_run(&_io_driver);
  }
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
shell_init(void)
{
  ESP_LOGI(TAG, "initializing shell");

  xTaskCreate(&shell_task, "shell_task", 4096, NULL, 5, NULL);
}
