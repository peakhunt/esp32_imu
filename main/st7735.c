#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "st7735.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

////////////////////////////////////////////////////////////////////////////////
//
// beginning of port layer
//
////////////////////////////////////////////////////////////////////////////////
#define SPI_MOSI_PIN              23
#define SPI_SCK_PIN               5
#define SPI_CS_PIN                16
#define ST7735_C_D_PIN            17
#define ST7735_RESET_PIN          9

#define ST7735_CS_LOW()       gpio_set_level(SPI_CS_PIN, 0)
#define ST7735_CS_HIGH()      gpio_set_level(SPI_CS_PIN, 1)

#define ST7735_RS_LOW()       gpio_set_level(ST7735_C_D_PIN, 0)
#define ST7735_RS_HIGH()      gpio_set_level(ST7735_C_D_PIN, 1)

//#define ST7735_RESET_LOW()    gpio_set_level(ST7735_RESET_PIN, 0)
//#define ST7735_RESET_HIGH()   gpio_set_level(ST7735_RESET_PIN, 1)
#define ST7735_RESET_LOW()
#define ST7735_RESET_HIGH()

#define ST7735_SPI_WRITE(c)   spi_sw_write(c)

#define osDelay(ms)           vTaskDelay(ms / portTICK_PERIOD_MS);

////////////////////////////////////////////////////////////////////////////////
//
// end of port layer
//
////////////////////////////////////////////////////////////////////////////////

static uint32_t StX = 0;
static uint32_t StY = 0; 
static uint16_t StTextColor = ST7735_YELLOW;
static int32_t Ymax,Ymin,X;
static int32_t Yrange; 
static unsigned char ColStart, RowStart; 
static unsigned char Rotation;
static enum initRFlags TabColor;
static short _width = ST7735_TFTWIDTH; 
static short _height = ST7735_TFTHEIGHT;

#define ST7735_NOP      0x00
#define ST7735_SWRESET  0x01
#define ST7735_RDDID    0x04
#define ST7735_RDDST    0x09

#define ST7735_SLPIN    0x10
#define ST7735_SLPOUT   0x11
#define ST7735_PTLON    0x12
#define ST7735_NORON    0x13

#define ST7735_INVOFF   0x20
#define ST7735_INVON    0x21
#define ST7735_DISPOFF  0x28
#define ST7735_DISPON   0x29
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E

#define ST7735_PTLAR    0x30
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36

#define ST7735_FRMCTR1  0xB1
#define ST7735_FRMCTR2  0xB2
#define ST7735_FRMCTR3  0xB3
#define ST7735_INVCTR   0xB4
#define ST7735_DISSET5  0xB6

#define ST7735_PWCTR1   0xC0
#define ST7735_PWCTR2   0xC1
#define ST7735_PWCTR3   0xC2
#define ST7735_PWCTR4   0xC3
#define ST7735_PWCTR5   0xC4
#define ST7735_VMCTR1   0xC5

#define ST7735_RDID1    0xDA
#define ST7735_RDID2    0xDB
#define ST7735_RDID3    0xDC
#define ST7735_RDID4    0xDD

#define ST7735_PWCTR6   0xFC

#define ST7735_GMCTRP1  0xE0
#define ST7735_GMCTRN1  0xE1

#define MADCTL_MY       0x80
#define MADCTL_MX       0x40
#define MADCTL_MV       0x20
#define MADCTL_ML       0x10
#define MADCTL_RGB      0x00
#define MADCTL_BGR      0x08
#define MADCTL_MH       0x04

// standard ascii 5x7 font
// originally from glcdfont.c from Adafruit project
static const unsigned char Font[] = 
{
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
  0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
  0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
  0x18, 0x3C, 0x7E, 0x3C, 0x18,
  0x1C, 0x57, 0x7D, 0x57, 0x1C,
  0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
  0x00, 0x18, 0x3C, 0x18, 0x00,
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
  0x00, 0x18, 0x24, 0x18, 0x00,
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
  0x30, 0x48, 0x3A, 0x06, 0x0E,
  0x26, 0x29, 0x79, 0x29, 0x26,
  0x40, 0x7F, 0x05, 0x05, 0x07,
  0x40, 0x7F, 0x05, 0x25, 0x3F,
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
  0x7F, 0x3E, 0x1C, 0x1C, 0x08,
  0x08, 0x1C, 0x1C, 0x3E, 0x7F,
  0x14, 0x22, 0x7F, 0x22, 0x14,
  0x5F, 0x5F, 0x00, 0x5F, 0x5F,
  0x06, 0x09, 0x7F, 0x01, 0x7F,
  0x00, 0x66, 0x89, 0x95, 0x6A,
  0x60, 0x60, 0x60, 0x60, 0x60,
  0x94, 0xA2, 0xFF, 0xA2, 0x94,
  0x08, 0x04, 0x7E, 0x04, 0x08,
  0x10, 0x20, 0x7E, 0x20, 0x10,
  0x08, 0x08, 0x2A, 0x1C, 0x08,
  0x08, 0x1C, 0x2A, 0x08, 0x08,
  0x1E, 0x10, 0x10, 0x10, 0x10,
  0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
  0x30, 0x38, 0x3E, 0x38, 0x30,
  0x06, 0x0E, 0x3E, 0x0E, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x5F, 0x00, 0x00,
  0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14,
  0x24, 0x2A, 0x7F, 0x2A, 0x12,
  0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x56, 0x20, 0x50,
  0x00, 0x08, 0x07, 0x03, 0x00,
  0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00,
  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
  0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x80, 0x70, 0x30, 0x00,
  0x08, 0x08, 0x08, 0x08, 0x08,
  0x00, 0x00, 0x60, 0x60, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02,
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x72, 0x49, 0x49, 0x49, 0x46, // 2
  0x21, 0x41, 0x49, 0x4D, 0x33, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x31, // 6
  0x41, 0x21, 0x11, 0x09, 0x07, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x46, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x14, 0x00, 0x00,
  0x00, 0x40, 0x34, 0x00, 0x00,
  0x00, 0x08, 0x14, 0x22, 0x41,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x41, 0x22, 0x14, 0x08,
  0x02, 0x01, 0x59, 0x09, 0x06,
  0x3E, 0x41, 0x5D, 0x59, 0x4E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x41, 0x3E, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x3E, 0x41, 0x41, 0x51, 0x73, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x1C, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x26, 0x49, 0x49, 0x49, 0x32, // S
  0x03, 0x01, 0x7F, 0x01, 0x03, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x59, 0x49, 0x4D, 0x43, // Z
  0x00, 0x7F, 0x41, 0x41, 0x41,
  0x02, 0x04, 0x08, 0x10, 0x20,
  0x00, 0x41, 0x41, 0x41, 0x7F,
  0x04, 0x02, 0x01, 0x02, 0x04,
  0x40, 0x40, 0x40, 0x40, 0x40,
  0x00, 0x03, 0x07, 0x08, 0x00,
  0x20, 0x54, 0x54, 0x78, 0x40, // a
  0x7F, 0x28, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x28, // c
  0x38, 0x44, 0x44, 0x28, 0x7F, // d
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x00, 0x08, 0x7E, 0x09, 0x02, // f
  0x18, 0xA4, 0xA4, 0x9C, 0x78, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x40, 0x3D, 0x00, // j
  0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x78, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0xFC, 0x18, 0x24, 0x24, 0x18, // p
  0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x24, // s
  0x04, 0x04, 0x3F, 0x44, 0x24, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x4C, 0x90, 0x90, 0x90, 0x7C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00,
  0x00, 0x00, 0x77, 0x00, 0x00,
  0x00, 0x41, 0x36, 0x08, 0x00,
  0x02, 0x01, 0x02, 0x04, 0x02,
  0x3C, 0x26, 0x23, 0x26, 0x3C,
  0x1E, 0xA1, 0xA1, 0x61, 0x12,
  0x3A, 0x40, 0x40, 0x20, 0x7A,
  0x38, 0x54, 0x54, 0x55, 0x59,
  0x21, 0x55, 0x55, 0x79, 0x41,
  0x21, 0x54, 0x54, 0x78, 0x41,
  0x21, 0x55, 0x54, 0x78, 0x40,
  0x20, 0x54, 0x55, 0x79, 0x40,
  0x0C, 0x1E, 0x52, 0x72, 0x12,
  0x39, 0x55, 0x55, 0x55, 0x59,
  0x39, 0x54, 0x54, 0x54, 0x59,
  0x39, 0x55, 0x54, 0x54, 0x58,
  0x00, 0x00, 0x45, 0x7C, 0x41,
  0x00, 0x02, 0x45, 0x7D, 0x42,
  0x00, 0x01, 0x45, 0x7C, 0x40,
  0xF0, 0x29, 0x24, 0x29, 0xF0,
  0xF0, 0x28, 0x25, 0x28, 0xF0,
  0x7C, 0x54, 0x55, 0x45, 0x00,
  0x20, 0x54, 0x54, 0x7C, 0x54,
  0x7C, 0x0A, 0x09, 0x7F, 0x49,
  0x32, 0x49, 0x49, 0x49, 0x32,
  0x32, 0x48, 0x48, 0x48, 0x32,
  0x32, 0x4A, 0x48, 0x48, 0x30,
  0x3A, 0x41, 0x41, 0x21, 0x7A,
  0x3A, 0x42, 0x40, 0x20, 0x78,
  0x00, 0x9D, 0xA0, 0xA0, 0x7D,
  0x39, 0x44, 0x44, 0x44, 0x39,
  0x3D, 0x40, 0x40, 0x40, 0x3D,
  0x3C, 0x24, 0xFF, 0x24, 0x24,
  0x48, 0x7E, 0x49, 0x43, 0x66,
  0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
  0xFF, 0x09, 0x29, 0xF6, 0x20,
  0xC0, 0x88, 0x7E, 0x09, 0x03,
  0x20, 0x54, 0x54, 0x79, 0x41,
  0x00, 0x00, 0x44, 0x7D, 0x41,
  0x30, 0x48, 0x48, 0x4A, 0x32,
  0x38, 0x40, 0x40, 0x22, 0x7A,
  0x00, 0x7A, 0x0A, 0x0A, 0x72,
  0x7D, 0x0D, 0x19, 0x31, 0x7D,
  0x26, 0x29, 0x29, 0x2F, 0x28,
  0x26, 0x29, 0x29, 0x29, 0x26,
  0x30, 0x48, 0x4D, 0x40, 0x20,
  0x38, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x38,
  0x2F, 0x10, 0xC8, 0xAC, 0xBA,
  0x2F, 0x10, 0x28, 0x34, 0xFA,
  0x00, 0x00, 0x7B, 0x00, 0x00,
  0x08, 0x14, 0x2A, 0x14, 0x22,
  0x22, 0x14, 0x2A, 0x14, 0x08,
  0xAA, 0x00, 0x55, 0x00, 0xAA,
  0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0x00, 0x00, 0x00, 0xFF, 0x00,
  0x10, 0x10, 0x10, 0xFF, 0x00,
  0x14, 0x14, 0x14, 0xFF, 0x00,
  0x10, 0x10, 0xFF, 0x00, 0xFF,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x14, 0x14, 0x14, 0xFC, 0x00,
  0x14, 0x14, 0xF7, 0x00, 0xFF,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x14, 0x14, 0xF4, 0x04, 0xFC,
  0x14, 0x14, 0x17, 0x10, 0x1F,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0x1F, 0x00,
  0x10, 0x10, 0x10, 0xF0, 0x00,
  0x00, 0x00, 0x00, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0xF0, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x10,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x10, 0x10, 0x10, 0xFF, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x14,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x00, 0x1F, 0x10, 0x17,
  0x00, 0x00, 0xFC, 0x04, 0xF4,
  0x14, 0x14, 0x17, 0x10, 0x17,
  0x14, 0x14, 0xF4, 0x04, 0xF4,
  0x00, 0x00, 0xFF, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x14, 0x14, 0xF7, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x17, 0x14,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0xF4, 0x14,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x00, 0x00, 0x1F, 0x10, 0x1F,
  0x00, 0x00, 0x00, 0x1F, 0x14,
  0x00, 0x00, 0x00, 0xFC, 0x14,
  0x00, 0x00, 0xF0, 0x10, 0xF0,
  0x10, 0x10, 0xFF, 0x10, 0xFF,
  0x14, 0x14, 0x14, 0xFF, 0x14,
  0x10, 0x10, 0x10, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0xF0, 0x10,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0xFF,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x38, 0x44, 0x44, 0x38, 0x44,
  0x7C, 0x2A, 0x2A, 0x3E, 0x14,
  0x7E, 0x02, 0x02, 0x06, 0x06,
  0x02, 0x7E, 0x02, 0x7E, 0x02,
  0x63, 0x55, 0x49, 0x41, 0x63,
  0x38, 0x44, 0x44, 0x3C, 0x04,
  0x40, 0x7E, 0x20, 0x1E, 0x20,
  0x06, 0x02, 0x7E, 0x02, 0x02,
  0x99, 0xA5, 0xE7, 0xA5, 0x99,
  0x1C, 0x2A, 0x49, 0x2A, 0x1C,
  0x4C, 0x72, 0x01, 0x72, 0x4C,
  0x30, 0x4A, 0x4D, 0x4D, 0x30,
  0x30, 0x48, 0x78, 0x48, 0x30,
  0xBC, 0x62, 0x5A, 0x46, 0x3D,
  0x3E, 0x49, 0x49, 0x49, 0x00,
  0x7E, 0x01, 0x01, 0x01, 0x7E,
  0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
  0x44, 0x44, 0x5F, 0x44, 0x44,
  0x40, 0x51, 0x4A, 0x44, 0x40,
  0x40, 0x44, 0x4A, 0x51, 0x40,
  0x00, 0x00, 0xFF, 0x01, 0x03,
  0xE0, 0x80, 0xFF, 0x00, 0x00,
  0x08, 0x08, 0x6B, 0x6B, 0x08,
  0x36, 0x12, 0x36, 0x24, 0x36,
  0x06, 0x0F, 0x09, 0x0F, 0x06,
  0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x00, 0x10, 0x10, 0x00,
  0x30, 0x40, 0xFF, 0x01, 0x01,
  0x00, 0x1F, 0x01, 0x01, 0x1E,
  0x00, 0x19, 0x1D, 0x17, 0x12,
  0x00, 0x3C, 0x3C, 0x3C, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0x00,
};

// full scaled defined as 3V
// Input is 0 to 511, 0 => 159 and 511 => 32
static uint8_t const dBfs[512]=
{
  159, 159, 145, 137, 131, 126, 123, 119, 117, 114, 112, 110, 108, 107, 105, 104, 103, 101, 
  100, 99, 98, 97, 96, 95, 94, 93, 93, 92, 91, 90, 90, 89, 88, 88, 87, 87, 86, 85, 85, 84, 
  84, 83, 83, 82, 82, 81, 81, 81, 80, 80, 79, 79, 79, 78, 78, 77, 77, 77, 76, 76, 76, 75, 
  75, 75, 74, 74, 74, 73, 73, 73, 72, 72, 72, 72, 71, 71, 71, 71, 70, 70, 70, 70, 69, 69, 
  69, 69, 68, 68, 68, 68, 67, 67, 67, 67, 66, 66, 66, 66, 66, 65, 65, 65, 65, 65, 64, 64, 
  64, 64, 64, 63, 63, 63, 63, 63, 63, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 60, 
  60, 60, 60, 60, 60, 59, 59, 59, 59, 59, 59, 59, 58, 58, 58, 58, 58, 58, 58, 57, 57, 57, 
  57, 57, 57, 57, 56, 56, 56, 56, 56, 56, 56, 56, 55, 55, 55, 55, 55, 55, 55, 55, 54, 54, 
  54, 54, 54, 54, 54, 54, 53, 53, 53, 53, 53, 53, 53, 53, 53, 52, 52, 52, 52, 52, 52, 52, 
  52, 52, 52, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 50, 50, 50, 50, 50, 50, 50, 50, 50, 
  50, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 
  48, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 46, 46, 46, 46, 46, 46, 46, 46, 46, 
  46, 46, 46, 46, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 44, 44, 44, 44, 44, 
  44, 44, 44, 44, 44, 44, 44, 44, 44, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 
  43, 43, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 41, 41, 41, 41, 41, 
  41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 
  40, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 
  39, 39, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 37, 
  37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 36, 36, 36, 36, 
  36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 35, 35, 35, 35, 35, 
  35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 34, 34, 34, 34, 34, 34, 
  34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33, 
  33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 32, 32, 32, 
  32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};

#define DELAY 0x80

static const unsigned char
  Bcmd[] = {                  // Initialization commands for 7735B screens
    18,                       // 18 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ST7735_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay:
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ST7735_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ST7735_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ST7735_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ST7735_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ST7735_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ST7735_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 };                  //     255 = 500 ms delay

static const unsigned char
  Rcmd1[] = {                 // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 };                 //     16-bit color

static const unsigned char
  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 };      //     XEND = 159

static const unsigned char
Rcmd2green144[] = {           // Init for 7735R, part 2 (green 1.44 tab)
  2,                          //  2 commands in list:
  ST7735_CASET  , 4      ,    //  1: Column addr set, 4 args, no delay:
    0x00, 0x00,               //     XSTART = 0
    0x00, 0x7F,               //     XEND = 127
  ST7735_RASET  , 4      ,    //  2: Row addr set, 4 args, no delay:
    0x00, 0x00,               //     XSTART = 0
    0x00, 0x7F };             //     XEND = 127

static const unsigned char
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F };           //     XEND = 159

static const unsigned char
  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


static inline void
spi_sw_write(unsigned char c)
{
  // MSB first
  for(int i = 0; i < 8; i++)
  {
    if((c & (0x80 >> i)))
    {
      // high
      gpio_set_level(SPI_MOSI_PIN, 1);
    }
    else
    {
      // low
      gpio_set_level(SPI_MOSI_PIN, 0);
    }
    
    // toggle clock
    gpio_set_level(SPI_SCK_PIN, 1);
    gpio_set_level(SPI_SCK_PIN, 0);
  }
}

static inline void
spi_sw_transmit(uint8_t* t, int n)
{
  for(int i =0; i < n; i++)
  {
    spi_sw_write(t[i]);
  }
}

static inline void
writecommand(unsigned char c)
{
  ST7735_RS_LOW();
  ST7735_CS_LOW();

  ST7735_SPI_WRITE(c);

  ST7735_CS_HIGH();
}

static inline void
writedata(unsigned char c)
{
  ST7735_RS_HIGH();
  ST7735_CS_LOW();

  ST7735_SPI_WRITE(c);

  ST7735_CS_HIGH();
}

static void
commandList(const unsigned char *addr)
{
  unsigned char numCommands, numArgs;
  unsigned short ms;

  numCommands = *(addr++);               // Number of commands to follow

  while(numCommands--)
  {                                      // For each command...
    writecommand(*(addr++));             //   Read, issue command
    numArgs  = *(addr++);                //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit

    while(numArgs--)
    {                                    //   For each argument...
      writedata(*(addr++));              //     Read, issue argument
    }

    if(ms)
    {
      ms = *(addr++);             // Read post-command delay time (ms)

      if(ms == 255)
        ms = 500;     // If 255, delay for 500 ms

      osDelay(ms);
    }
  }
}

static void 
commonInit(const unsigned char *cmdList)
{
  ColStart  = RowStart = 0; // May be overridden in init func
  StX = StY = 0;

  ST7735_CS_LOW();
  ST7735_RESET_HIGH();
  osDelay(500);

  ST7735_RESET_LOW();
  osDelay(500);

  ST7735_RESET_HIGH();
  osDelay(500);

  if(cmdList)
    commandList(cmdList);

  ST7735_CS_HIGH();
}

static void
st7735_init(void)
{
  gpio_pad_select_gpio(SPI_MOSI_PIN);
  gpio_set_direction(SPI_MOSI_PIN, GPIO_MODE_OUTPUT);

  gpio_pad_select_gpio(SPI_SCK_PIN);
  gpio_set_direction(SPI_SCK_PIN, GPIO_MODE_OUTPUT);

  gpio_pad_select_gpio(SPI_CS_PIN);
  gpio_set_direction(SPI_CS_PIN, GPIO_MODE_OUTPUT);

  gpio_pad_select_gpio(ST7735_C_D_PIN);
  gpio_set_direction(ST7735_C_D_PIN, GPIO_MODE_OUTPUT);

  /*
  gpio_pad_select_gpio(ST7735_RESET_PIN);
  gpio_set_direction(ST7735_RESET_PIN, GPIO_MODE_OUTPUT);
  */

  gpio_set_level(SPI_CS_PIN, 1);
  gpio_set_level(SPI_SCK_PIN, 0);
}

void
st7735_initb(void)
{
  st7735_init();

  commonInit(Bcmd);
}

void
st7735_initr(enum initRFlags option)
{
  st7735_init();

  commonInit(Rcmd1);

  if(option == INITR_GREENTAB)
  {
    commandList(Rcmd2green);
    ColStart = 2;
    RowStart = 1;
  }
  else if(option == INITR_144GREENTAB)
  {
    commandList(Rcmd2green144);
    ColStart = 2;
    RowStart = 3;
  }
  else
  {
    // colstart, rowstart left at default '0' values
    commandList(Rcmd2red);
  }
  commandList(Rcmd3);

  // if black, change MADCTL color filter
  if (option == INITR_BLACKTAB)
  {
    writecommand(ST7735_MADCTL);
    writedata(0xC0);
  }
  TabColor = option;
}

static inline void 
set_addr_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  writecommand(ST7735_CASET); // Column addr set
  writedata(0x00);
  writedata(x0+ColStart);     // XSTART
  writedata(0x00);
  writedata(x1+ColStart);     // XEND

  writecommand(ST7735_RASET); // Row addr set
  writedata(0x00);
  writedata(y0+RowStart);     // YSTART
  writedata(0x00);
  writedata(y1+RowStart);     // YEND

  writecommand(ST7735_RAMWR); // write to RAM
}

static inline void
pushColor(uint16_t color)
{
  writedata((uint8_t)(color >> 8));
  writedata((uint8_t)color);
}


void
st7735_drawpixel(int16_t x, int16_t y, uint16_t color)
{
  if((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return;

  set_addr_window(x,y,x,y);
  pushColor(color);
}

void
st7735_drawfastvline(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  uint8_t hi = color >> 8, lo = color;

  if((x >= _width) || (y >= _height))
    return;

  if((y+h-1) >= _height)
    h = _height-y;

  set_addr_window(x, y, x, y+h-1);

  while (h--)
  {
    writedata(hi);
    writedata(lo);
  }
}

void
st7735_drawfasthline(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  uint8_t hi = color >> 8, lo = color;

  if((x >= _width) || (y >= _height))
    return;

  if((x+w-1) >= _width)  w = _width-x;

  set_addr_window(x, y, x+w-1, y);

  while (w--)
  {
    writedata(hi);
    writedata(lo);
  }
}

void
st7735_fillscreen(uint16_t color)
{
  st7735_fillrect(0, 0, _width, _height, color);
}

void
st7735_fillrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  uint8_t hi = color >> 8, lo = color;

  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  set_addr_window(x, y, x+w-1, y+h-1);

  for(y=h; y>0; y--)
  {
    for(x=w; x>0; x--)
    {
      writedata(hi);
      writedata(lo);
    }
  }
}

uint16_t
st7735_color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
}

uint16_t
st7735_swapcolor(uint16_t x)
{
  return (x << 11) | (x & 0x07E0) | (x >> 11);
}

void
st7735_drawbitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h)
{
  int16_t skipC = 0;
  int16_t originalWidth = w;
  int i = w*(h - 1);

  if((x >= _width) || ((y - h + 1) >= _height) || ((x + w) <= 0) || (y < 0))
  {
    return;
  }

  if((w > _width) || (h > _height))
  { 
    return;
  }

  if((x + w - 1) >= _width)
  {                                     // image exceeds right of screen
    skipC = (x + w) - _width;           // skip cut off columns
    w = _width - x;
  }

  if((y - h + 1) < 0)
  {                                     // image exceeds top of screen
    i = i - (h - y - 1)*originalWidth;  // skip the last cut off rows
    h = y + 1;
  }

  if(x < 0)
  {                                     // image exceeds left of screen
    w = w + x;
    skipC = -1*x;                       // skip cut off columns
    i = i - x;                          // skip the first cut off columns
    x = 0;
  }

  if(y >= _height)
  {                                     // image exceeds bottom of screen
    h = h - (y - _height + 1);
    y = _height - 1;
  }

  set_addr_window(x, y-h+1, x+w-1, y);

  for(y=0; y<h; y=y+1)
  {
    for(x=0; x<w; x=x+1)
    {
      // send the top 8 bits
      writedata((uint8_t)(image[i] >> 8));
      // send the bottom 8 bits
      writedata((uint8_t)image[i]);
      i = i + 1;                        // go to the next pixel
    }
    i = i + skipC;
    i = i - 2*originalWidth;
  }
}

void
st7735_drawchars(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size)
{
  uint8_t line; // vertical column of pixels of character in font

  int32_t i, j;
  if((x >= _width)            || // Clip right
      (y >= _height)           || // Clip bottom
      ((x + 5 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

  for (i=0; i<6; i++ ) {
    if (i == 5)
      line = 0x0;
    else
      line = Font[(c*5)+i];
    for (j = 0; j<8; j++) {
      if (line & 0x1) {
        if (size == 1) // default size
          st7735_drawpixel(x+i, y+j, textColor);
        else {  // big size
          st7735_fillrect(x+(i*size), y+(j*size), size, size, textColor);
        }
      } else if (bgColor != textColor) {
        if (size == 1) // default size
          st7735_drawpixel(x+i, y+j, bgColor);
        else {  // big size
          st7735_fillrect(x+i*size, y+j*size, size, size, bgColor);
        }
      }
      line >>= 1;
    }
  }
}

void
st7735_drawchar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size)
{
  uint8_t line; // horizontal row of pixels of character
  int32_t col, row, i, j;// loop indices
  if(((x + 5*size - 1) >= _width)  || // Clip right
      ((y + 8*size - 1) >= _height) || // Clip bottom
      ((x + 5*size - 1) < 0)        || // Clip left
      ((y + 8*size - 1) < 0)){         // Clip top
    return;
  }

  set_addr_window(x, y, x+6*size-1, y+8*size-1);

  line = 0x01;        // print the top row first
  // print the rows, starting at the top
  for(row=0; row<8; row=row+1){
    for(i=0; i<size; i=i+1){
      // print the columns, starting on the left
      for(col=0; col<5; col=col+1){
        if(Font[(c*5)+col]&line){
          // bit is set in Font, print pixel(s) in text color
          for(j=0; j<size; j=j+1){
            pushColor(textColor);
          }
        } else{
          // bit is cleared in Font, print pixel(s) in background color
          for(j=0; j<size; j=j+1){
            pushColor(bgColor);
          }
        }
      }
      // print blank column(s) to the right of character
      for(j=0; j<size; j=j+1){
        pushColor(bgColor);
      }
    }
    line = line<<1;   // move up to the next row
  }
}

uint32_t st7735_drawstring(uint16_t x, uint16_t y, char *pt, int16_t textColor)
{
  uint32_t count = 0;
  if(y>15) return 0;
  while(*pt){
    st7735_drawchars(x*6, y*10, *pt, textColor, ST7735_BLACK, 1);
    pt++;
    x = x+1;
    if(x>20) return count;  // number of characters printed
    count++;
  }
  return count;  // number of characters printed
}

char Message[12];
uint32_t Messageindex;

void fillmessage(uint32_t n)
{
  if(n >= 10){
    fillmessage(n/10);
    n = n%10;
  }
  Message[Messageindex] = (n+'0'); /* n is between 0 and 9 */
  if(Messageindex<11)Messageindex++;
}

void st7735_setcursor(uint32_t newX, uint32_t newY){
  if((newX > 20) || (newY > 15)){       // bad input
    return;                             // do nothing
  }
  StX = newX;
  StY = newY;
}

void
st7735_outudec(uint32_t n)
{
  Messageindex = 0;
  fillmessage(n);
  Message[Messageindex] = 0; // terminate

  st7735_drawstring(StX,StY,Message,StTextColor);

  StX = StX+Messageindex;

  if(StX>20)
  {
    StX = 20;
    st7735_drawchars(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }
}

void
st7735_setrotation(uint8_t m)
{

  writecommand(ST7735_MADCTL);
  Rotation = m % 4; // can't be higher than 3
  switch (Rotation)
  {
  case 0:
    if (TabColor == INITR_BLACKTAB)
    {
      writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
    }
    else
    {
      writedata(MADCTL_MX | MADCTL_MY | MADCTL_BGR);
    }
    _width  = ST7735_TFTWIDTH;
    _height = ST7735_TFTHEIGHT;
    break;

  case 1:
    if (TabColor == INITR_BLACKTAB)
    {
      writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
    }
    else
    {
      writedata(MADCTL_MY | MADCTL_MV | MADCTL_BGR);
    }
    _width  = ST7735_TFTHEIGHT;
    _height = ST7735_TFTWIDTH;
    break;

  case 2:
    if (TabColor == INITR_BLACKTAB)
    {
      writedata(MADCTL_RGB);
    }
    else
    {
      writedata(MADCTL_BGR);
    }
    _width  = ST7735_TFTWIDTH;
    _height = ST7735_TFTHEIGHT;
    break;

  case 3:
    if (TabColor == INITR_BLACKTAB)
    {
      writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
    }
    else
    {
      writedata(MADCTL_MX | MADCTL_MV | MADCTL_BGR);
    }
    _width  = ST7735_TFTHEIGHT;
    _height = ST7735_TFTWIDTH;
    break;
  }
}

void st7735_invertdisplay(int i)
{
  if(i)
  {
    writecommand(ST7735_INVON);
  }
  else
  {
    writecommand(ST7735_INVOFF);
  }
}

void st7735_plotclear(int32_t ymin, int32_t ymax){
  st7735_fillrect(0, 32, 128, 128, st7735_color565(228,228,228)); // light grey
  if(ymax>ymin){
    Ymax = ymax;
    Ymin = ymin;
    Yrange = ymax-ymin;
  } else{
    Ymax = ymin;
    Ymin = ymax;
    Yrange = ymax-ymin;
  }
  X = 0;
}

void st7735_plotpoint(int32_t y)
{
  int32_t j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;
  j = 32+(127*(Ymax-y))/Yrange;
  if(j<32) j = 32;
  if(j>159) j = 159;
  st7735_drawpixel(X,   j,   ST7735_BLUE);
  st7735_drawpixel(X+1, j,   ST7735_BLUE);
  st7735_drawpixel(X,   j+1, ST7735_BLUE);
  st7735_drawpixel(X+1, j+1, ST7735_BLUE);
}

int32_t lastj=0;
void st7735_plotline(int32_t y)
{
  int32_t i,j;
  if(y<Ymin) y=Ymin;
  if(y>Ymax) y=Ymax;

  j = 32+(127*(Ymax-y))/Yrange;
  if(j < 32) j = 32;
  if(j > 159) j = 159;
  if(lastj < 32) lastj = j;
  if(lastj > 159) lastj = j;
  if(lastj < j){
    for(i = lastj+1; i<=j ; i++){
      st7735_drawpixel(X,   i,   ST7735_BLUE) ;
      st7735_drawpixel(X+1, i,   ST7735_BLUE) ;
    }
  }else if(lastj > j){
    for(i = j; i<lastj ; i++){
      st7735_drawpixel(X,   i,   ST7735_BLUE) ;
      st7735_drawpixel(X+1, i,   ST7735_BLUE) ;
    }
  }else{
    st7735_drawpixel(X,   j,   ST7735_BLUE) ;
    st7735_drawpixel(X+1, j,   ST7735_BLUE) ;
  }
  lastj = j;
}

void
st7735_plotpoints(int32_t y1,int32_t y2)
{
  int32_t j;

  if(y1<Ymin)
    y1=Ymin;

  if(y1>Ymax)
    y1=Ymax;

  j = 32+(127*(Ymax-y1))/Yrange;

  if(j<32)
    j = 32;

  if(j>159)
    j = 159;

  st7735_drawpixel(X, j, ST7735_BLUE);

  if(y2<Ymin)
    y2=Ymin;

  if(y2>Ymax)
    y2=Ymax;

  j = 32+(127*(Ymax-y2))/Yrange;

  if(j<32)
    j = 32;

  if(j>159)
    j = 159;

  st7735_drawpixel(X, j, ST7735_BLACK);
}

void st7735_plotbar(int32_t y)
{
  int32_t j;

  if(y<Ymin)
    y=Ymin;

  if(y>Ymax)
    y=Ymax;

  j = 32+(127*(Ymax-y))/Yrange;
  st7735_drawfastvline(X, j, 159-j, ST7735_BLACK);
}

void
st7735_plotdbfs(int32_t y)
{
  int32_t j;

  y = y/2; // 0 to 2047

  if(y<0)
    y=0;

  if(y>511)
    y=511;

  j = dBfs[y];
  st7735_drawfastvline(X, j, 159-j, ST7735_BLACK);
}
void
st7735_plotnext(void)
{
  if(X==127)
  {
    X = 0;
  }
  else
  {
    X++;
  }
}

void
st7735_plotnexterase(void)
{
  if(X==127)
  {
    X = 0;
  }
  else
  {
    X++;
  }
  st7735_drawfastvline(X,32,128,st7735_color565(228,228,228));
}

void
st7735_outchar(char ch)
{
  if((ch == 10) || (ch == 13) || (ch == 27))
  {
    StY++; StX=0;
    if(StY>15)
    {
      StY = 0;
    }
    st7735_drawstring(0,StY,"                     ",StTextColor);
    return;
  }
  
  st7735_drawchars(StX*6,StY*10,ch,ST7735_YELLOW,ST7735_BLACK, 1);
  StX++;
  if(StX>20)
  {
    StX = 20;
    st7735_drawchars(StX*6,StY*10,'*',ST7735_RED,ST7735_BLACK, 1);
  }
  return;
}

void
st7735_outstring(char *ptr)
{
  while(*ptr)
  {
    st7735_outchar(*ptr);
    ptr = ptr + 1;
  }
}

void
st7735_settextcolor(uint16_t color)
{
  StTextColor = color;
}

void
st7735_output_init(void)
{
  st7735_initr(INITR_REDTAB);
  st7735_fillscreen(0);                 // set screen to black
}

void
st7735_output_clear(void)
{
  st7735_fillscreen(0);    // set screen to black
}

void
st7735_output_off(void)
{
  st7735_output_clear();  // not implemented
  // writecommand(0x10);   // sleep in
}

void
st7735_output_on(void)
{
  st7735_output_init();      // reinitialize
  // writecommand(0x11);   // sleep in
}

void
st7735_output_color(uint32_t newColor)
{
  // Set color of future output 
  st7735_settextcolor(newColor);
}
