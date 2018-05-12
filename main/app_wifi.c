#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

#define TARGET_WIFI_SSID      CONFIG_WIFI_SSID
#define TARGET_WIFI_PASS      CONFIG_WIFI_PASSWORD

static const char* TAG = "app_wifi";


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
    ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
    ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
    break;

  case SYSTEM_EVENT_STA_DISCONNECTED:
    // FIXME
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
  esp_err_t ret = nvs_flash_init();
  if(ret == ESP_ERR_NVS_NO_FREE_PAGES) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

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
