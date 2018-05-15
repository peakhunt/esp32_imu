#ifndef __ST7735_DEF_H__
#define __ST7735_DEF_H__

enum initRFlags
{
  none,
  INITR_GREENTAB,
  INITR_REDTAB,
  INITR_BLACKTAB,
  INITR_144GREENTAB
};

#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 128


// Color definitions
#define ST7735_BLACK    0x0000
#define ST7735_BLUE     0x001F
#define ST7735_RED      0xF800
#define ST7735_GREEN    0x07E0
#define ST7735_CYAN     0x07FF
#define ST7735_MAGENTA  0xF81F
#define ST7735_YELLOW   0xFFE0
#define ST7735_WHITE    0xFFFF

void st7735_initb(void);
void st7735_initr(enum initRFlags option);
void st7735_drawpixel(int16_t x, int16_t y, uint16_t color);
void st7735_drawfastvline(int16_t x, int16_t y, int16_t h, uint16_t color);
void st7735_drawfasthline(int16_t x, int16_t y, int16_t w, uint16_t color);
void st7735_fillscreen(uint16_t color);
void st7735_fillrect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
uint16_t st7735_color565(uint8_t r, uint8_t g, uint8_t b);
uint16_t st7735_swapcolor(uint16_t x) ;
void st7735_drawbitmap(int16_t x, int16_t y, const uint16_t *image, int16_t w, int16_t h);
void st7735_drawchars(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);
void st7735_drawchar(int16_t x, int16_t y, char c, int16_t textColor, int16_t bgColor, uint8_t size);
uint32_t st7735_drawstring(uint16_t x, uint16_t y, char *pt, int16_t textColor);;
void st7735_setcursor(uint32_t newX, uint32_t newY);
void st7735_outudec(uint32_t n);
void st7735_setrotation(uint8_t m) ;
void st7735_invertdisplay(int i) ;
void st7735_plotclear(int32_t ymin, int32_t ymax);
void st7735_plotpoint(int32_t y);
void st7735_plotline(int32_t y);
void st7735_plotpoints(int32_t y1,int32_t y2);
void st7735_plotbar(int32_t y);
void st7735_plotdbfs(int32_t y);
void st7735_plotnext(void);
void st7735_plotnexterase(void);
void st7735_outchar(char ch);
void st7735_outstring(char *ptr);
void st7735_settextcolor(uint16_t color);
void st7735_output_init(void);
void st7735_output_clear(void);
void st7735_output_off(void);
void st7735_output_on(void);
void st7735_output_color(uint32_t newColor); 

#endif /* !__ST7735_DEF_H__ */
