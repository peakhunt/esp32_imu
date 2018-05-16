#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "app_wifi.h"

#define TARGET_WIFI_SSID      CONFIG_WIFI_SSID
#define TARGET_WIFI_PASS      CONFIG_WIFI_PASSWORD

static const char* TAG = "app_wifi";
static tcpip_adapter_ip_info_t    _ip_info;
static bool                       _ip_configured = FALSE;
static SemaphoreHandle_t          _mutex;

static wifi_config_t              _wifi_config =
{
  .sta = {
    .ssid     = CONFIG_WIFI_SSID,
    .password = CONFIG_WIFI_PASSWORD,
  },
};

static void
update_ip_info(const tcpip_adapter_ip_info_t* info)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);
  
  _ip_configured = TRUE;
  memcpy(&_ip_info, info, sizeof(_ip_info));

  xSemaphoreGive(_mutex);
}

static void
get_ip_info(tcpip_adapter_ip_info_t* info, bool* is_configured)
{
  xSemaphoreTake(_mutex, portMAX_DELAY);
  
  *is_configured = _ip_configured;
  memcpy(info, &_ip_info, sizeof(_ip_info));

  xSemaphoreGive(_mutex);
}

static esp_err_t
wifi_event_handler(void* ctx, system_event_t* event)
{
  switch(event->event_id)
  {
  case SYSTEM_EVENT_STA_START:
    // FIXME
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_START");
    ESP_ERROR_CHECK(esp_wifi_connect());      // ??? why ???
    break;

  case SYSTEM_EVENT_STA_GOT_IP:
    // FIXME
    update_ip_info(&event->event_info.got_ip.ip_info);

    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    // FIXME
    _ip_configured = FALSE;

    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_DISCONNECTED");
    ESP_ERROR_CHECK(esp_wifi_connect());
    break;

  default:
    break;
  }

  return ESP_OK;
}

static void
app_wifi_read_config(void)
{
  esp_err_t   err;
  nvs_handle  my_handle;
  size_t      len;

  err = nvs_open("storage", NVS_READWRITE,  &my_handle);

  if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    return;
  }

  err = nvs_get_str(my_handle, "ap", NULL, &len);
  if(err == ESP_OK)
  {
    nvs_get_str(my_handle, "ap", (char*)_wifi_config.sta.ssid, &len);
  }

  err = nvs_get_str(my_handle, "pass", NULL, &len);
  if(err == ESP_OK)
  {
    nvs_get_str(my_handle, "pass", (char*)_wifi_config.sta.password, &len);
  }

  nvs_close(my_handle);
}

void
app_wifi_init(void)
{
  _mutex = xSemaphoreCreateMutex();

  app_wifi_read_config();

	tcpip_adapter_init();

  ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));

  ESP_LOGI(TAG, "initializing wifi");

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  ESP_LOGI(TAG, "starting wifi: ssid %s", _wifi_config.sta.ssid);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &_wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void
app_wifi_get_config(tcpip_adapter_ip_info_t* info, char ssid[32], bool* is_configured)
{
  strcpy(ssid, (char*)_wifi_config.sta.ssid);
  get_ip_info(info, is_configured);
}
