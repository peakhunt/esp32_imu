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
#include "sdkconfig.h"

#include "cli.h"

#include "esp_system.h"
#include "esp_spi_flash.h"

#include "app_wifi.h"
#include "imu_task.h"

////////////////////////////////////////////////////////////////////////////////
//
// private prototypes
//
////////////////////////////////////////////////////////////////////////////////
static void cli_command_sysinfo(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_systime(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_ipinfo(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_nvs(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_restart(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_wifi(cli_intf_t* intf, int argc, const char** argv);
static void cli_command_imu_data(cli_intf_t* intf, int argc, const char** argv);

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
  },
  {
    "ipinfo",
    "show IP address information",
    cli_command_ipinfo,
  },
  {
    "nvs",
    "nvs manipulation command",
    cli_command_nvs,
  },
  {
    "restart",
    "restart the chip",
    cli_command_restart,
  },
  {
    "wifi",
    "configure wifi STA",
    cli_command_wifi,
  },
  {
    "imu_data",
    "show IMU data",
    cli_command_imu_data,
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

static void
cli_command_ipinfo(cli_intf_t* intf, int argc, const char** argv)
{
  tcpip_adapter_ip_info_t   info;
  bool                      is_configured;
  char                      ssid[36];

  app_wifi_get_config(&info,  ssid, &is_configured);

  cli_printf(intf, CLI_EOL);

  cli_printf(intf, "SSID         : %s"CLI_EOL, ssid);

  if(is_configured)
  {
    cli_printf(intf, "IP Address   : %s"CLI_EOL, ip4addr_ntoa(&info.ip));
    cli_printf(intf, "Subnet Mask  : %s"CLI_EOL, ip4addr_ntoa(&info.netmask));
    cli_printf(intf, "Gateway      : %s"CLI_EOL, ip4addr_ntoa(&info.gw));
  }
  else
  {
    cli_printf(intf, "IP address is not yet configured"CLI_EOL);
  }
}

static void
cli_command_nvs(cli_intf_t* intf, int argc, const char** argv)
{
  typedef enum {
    nvs_read,
    nvs_write,
    nvs_erase,
  } nvs_op_t;

  nvs_op_t    op;
  esp_err_t   err;
  nvs_handle  my_handle;

  cli_printf(intf, CLI_EOL);

  if(argc < 3) goto command_error;

  if(strcmp(argv[1], "read") == 0)
  {
    if(argc != 4) goto command_error;

    op = nvs_read;
  }
  else if(strcmp(argv[1], "write") == 0)
  {
    if(argc != 5) goto command_error;

    op = nvs_write;
  }
  else if(strcmp(argv[1], "erase") == 0)
  {
    if(argc != 3) goto command_error;

    op = nvs_erase;
  }
  else
  {
    goto command_error;
  }

  if(op == nvs_read || op == nvs_write)
  {
    if(!(strcmp(argv[2], "int")  == 0 || strcmp(argv[2], "str") == 0)) goto command_error;
  }

  err = nvs_open("storage", NVS_READWRITE,  &my_handle);
  if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    cli_printf(intf, "Failed to open NVS"CLI_EOL);
    return;
  }

  switch(op)
  {
  case nvs_read:
    if(argv[2][0] == 'i')
    {
      int32_t   v;

      err = nvs_get_i32(my_handle, argv[3], &v);
      if(err == ESP_OK)
      {
        cli_printf(intf, "%s: %d"CLI_EOL, argv[3], v);
      }
    }
    else
    {
      size_t     len;

      err = nvs_get_str(my_handle, argv[3], NULL, &len);
      if(err == ESP_OK)
      {
        char*   v;

        v = malloc(len);
        nvs_get_str(my_handle, argv[3], v, &len);

        cli_printf(intf, "%s: %s"CLI_EOL, argv[3], v);

        free(v);
      }
    }

    if(err != ESP_OK)
    {
      cli_printf(intf, "failed to get %s"CLI_EOL, argv[3]);
    }
    break;

  case nvs_write:
    if(argv[2][0] == 'i')
    {
      int32_t   v = atoi(argv[4]);

      err = nvs_set_i32(my_handle, argv[3], v);
      nvs_commit(my_handle);

      if(err == ESP_OK)
      {
        cli_printf(intf, "set %s to %d"CLI_EOL, argv[3], v);
      }
      else
      {
        cli_printf(intf, "failed to set %s to %d"CLI_EOL, argv[3], v);
      }
    }
    else
    {
      err = nvs_set_str(my_handle, argv[3], argv[4]);
      nvs_commit(my_handle);

      if(err == ESP_OK)
      {
        cli_printf(intf, "set %s to %s"CLI_EOL, argv[3], argv[4]);
      }
      else
      {
        cli_printf(intf, "failed set %s to %s"CLI_EOL, argv[3], argv[4]);
      }
    }
    break;

  case nvs_erase:
    err = nvs_erase_key(my_handle, argv[2]);
    nvs_commit(my_handle);

    if(err == ESP_OK)
    {
      cli_printf(intf, "deleted %s"CLI_EOL, argv[2]);
    }
    else
    {
      cli_printf(intf, "failed to delete %s"CLI_EOL, argv[2]);
    }
    break;
  }

  nvs_close(my_handle);
  return;

command_error:
  cli_printf(intf, "command error"CLI_EOL);
  cli_printf(intf, "nvs read [int|str] name"CLI_EOL);
  cli_printf(intf, "nvs write [int|str] name value"CLI_EOL);
  cli_printf(intf, "nvs erase name"CLI_EOL);
}

static void
cli_command_restart(cli_intf_t* intf, int argc, const char** argv)
{
  esp_restart();
}

static void
cli_command_wifi(cli_intf_t* intf, int argc, const char** argv)
{
  esp_err_t   err;
  nvs_handle  my_handle;

  if(argc < 2) goto command_error;

  err = nvs_open("storage", NVS_READWRITE,  &my_handle);
  if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    cli_printf(intf, "Failed to open NVS"CLI_EOL);
    return;
  }

  nvs_set_str(my_handle, "ap", argv[1]);
  if(argc == 3)
  {
    nvs_set_str(my_handle, "pass", argv[2]);
  }
  else
  {
    char t[1] = "\0";
    nvs_set_str(my_handle, "pass", t);
  }

  nvs_commit(my_handle);
  nvs_close(my_handle);

  return;

command_error:
  cli_printf(intf, "command error"CLI_EOL);
  cli_printf(intf, "wifi <ssid> [optional password]"CLI_EOL);
}

static void
cli_command_imu_data(cli_intf_t* intf, int argc, const char** argv)
{
  imu_sensor_data_t raw;
  imu_data_t        data;

  imu_task_get_raw_and_data(&raw, &data);

  cli_printf(intf, "Accel Raw X: %d"CLI_EOL, raw.accel[0]);
  cli_printf(intf, "Accel Raw Y: %d"CLI_EOL, raw.accel[1]);
  cli_printf(intf, "Accel Raw Z: %d"CLI_EOL, raw.accel[2]);

  cli_printf(intf, "Accel X    : %-5.2f G"CLI_EOL, data.accel[0]);
  cli_printf(intf, "Accel Y    : %-5.2f G"CLI_EOL, data.accel[1]);
  cli_printf(intf, "Accel Z    : %-5.2f G"CLI_EOL, data.accel[2]);

  cli_printf(intf, "Gyro  Raw X: %d"CLI_EOL, raw.gyro[0]);
  cli_printf(intf, "Gyro  Raw Y: %d"CLI_EOL, raw.gyro[1]);
  cli_printf(intf, "Gyro  Raw Z: %d"CLI_EOL, raw.gyro[2]);
  cli_printf(intf, "gyro X     : %-5.2f deg/S"CLI_EOL, data.gyro[0]);
  cli_printf(intf, "gyro Y     : %-5.2f deg/S"CLI_EOL, data.gyro[1]);
  cli_printf(intf, "gyro Z     : %-5.2f deg/S"CLI_EOL, data.gyro[2]);

  cli_printf(intf, "Mag   Raw X: %d"CLI_EOL, raw.mag[0]);
  cli_printf(intf, "Mag   Raw Y: %d"CLI_EOL, raw.mag[1]);
  cli_printf(intf, "Mag   Raw Z: %d"CLI_EOL, raw.mag[2]);
  cli_printf(intf, "Mag  X     : %-5.2f uT"CLI_EOL, data.mag[0]);
  cli_printf(intf, "Mag  Y     : %-5.2f uT"CLI_EOL, data.mag[1]);
  cli_printf(intf, "Mag  Z     : %-5.2f uT"CLI_EOL, data.mag[2]);

  cli_printf(intf, "T RAW      : %d"CLI_EOL, raw.temp);
  cli_printf(intf, "Temp       : %-5.2f Celcius"CLI_EOL, data.temp);
  cli_printf(intf, "Loop Count : %u"CLI_EOL, imu_task_get_loop_cnt());
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
