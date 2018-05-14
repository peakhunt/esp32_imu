#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "blinky.h"
#include "app_wifi.h"
#include "shell.h"
#include "lcd_driver.h"

void app_main()
{
  app_wifi_init();
  blinky_init();
  lcd_driver_init();
  shell_init();
}
