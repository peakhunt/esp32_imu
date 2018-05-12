# ESP32 IMU

9-DOF IMU using ESP32 and MPU9250

![esp32 ttgo](captures/esp32_ttgo.jpg "esp32 ttgo")

## current status as of May,12,2018

just getting started, folks!

## Pins
Green LED     => 22

## Boot Process

FIXME

## Debugger Setup

1. get openocd
  * [well explained here](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/#jtag-debugging-setup-openocd)
  * [download esp32 openocd](http://esp-idf.readthedocs.io/en/latest/api-guides/jtag-debugging/setup-openocd-linux.html)

2. FTDI Connection
  | FT2232HL     |  ESP32     | Purpose    |
  | ------------ | ---------- | ---------- |
  |ADBUS0        | 13         | TCK        |
  |ADBUS1        | 12         | TDI        |
  |ADBUS2        | 15         | TDO        |
  |ADBUS3        | 14         | TMS        |
  |GND           |            | GND        |
  |VIO           |            | 3V3        |


  Problem is Pin 12/13/14/15 are used for SDIO, which means
  * you can't use JTAG interface while using Micro-SD slot
  * we need a some special SDIO adapter to connect FT2232H and ESP32
