#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "st7735.h"
#include "app_wifi.h"

static const char* TAG = "lcd driver";

void print_page1(void)
{
  tcpip_adapter_ip_info_t   info;
  bool                      is_configured;
  char                      buffer[32];

  app_wifi_get_config(&info,  &is_configured);

  st7735_drawstring(0, 0, "System Ready         ", ST7735_WHITE);

  if(is_configured)
  {
    st7735_drawstring(0, 1, "IP Addreess Info     ", ST7735_WHITE);

    sprintf(buffer, "IP   : %-14s", ip4addr_ntoa(&info.ip));
    st7735_drawstring(0, 2, buffer, ST7735_WHITE);

    sprintf(buffer, "Mask : %-14s", ip4addr_ntoa(&info.netmask));
    st7735_drawstring(0, 3, buffer, ST7735_WHITE);

    sprintf(buffer, "GW   : %-14s", ip4addr_ntoa(&info.gw));
    st7735_drawstring(0, 4, buffer, ST7735_WHITE);
  }
  else
  {
    st7735_drawstring(0, 1, "Obtaining IP Address ", ST7735_WHITE);
  }
}

void lcd_driver_task(void *pvParameter)
{
  st7735_initr(INITR_144GREENTAB);
  st7735_fillscreen(ST7735_BLACK);

  st7735_setrotation(3) ;

  st7735_drawstring(0, 0, "Booting up...        ", ST7735_WHITE);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  
  while(1)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    print_page1();
  }
}

void
lcd_driver_init(void)
{
  ESP_LOGI(TAG, "starting lcd driver");

  xTaskCreate(lcd_driver_task, "lcd_driver_task", 4096, NULL, 5, NULL);
}
