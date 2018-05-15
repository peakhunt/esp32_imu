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

#include "app_wifi.h"

#define TARGET_WIFI_SSID      CONFIG_WIFI_SSID
#define TARGET_WIFI_PASS      CONFIG_WIFI_PASSWORD

static const char* TAG = "app_wifi";
static tcpip_adapter_ip_info_t    _ip_info;
static bool                       _ip_configured = FALSE;
static SemaphoreHandle_t          _mutex;

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

void
app_wifi_init(void)
{
  _mutex = xSemaphoreCreateMutex();

	tcpip_adapter_init();

  ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));

  ESP_LOGI(TAG, "initializing wifi");

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );

  wifi_config_t wifi_config = {
    .sta = {
      .ssid = TARGET_WIFI_SSID,
      .password = TARGET_WIFI_PASS,
    },
  };

  ESP_LOGI(TAG, "starting wifi: ssid %s", wifi_config.sta.ssid);

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());
}

void
app_wifi_get_config(tcpip_adapter_ip_info_t* info, bool* is_configured)
{
  get_ip_info(info, is_configured);
}
