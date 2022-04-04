#include <stdint.h>
#include <stdbool.h>
#include "../../uart.h"
#include "lcd.h"
#include "lcdfont.h"
#include "../led/led.h"

/*
TODO:
1. Replace delay_ms with LIBFEMTORV32 wait_cyles/milli_wait
2. Implement address set = DONE
3. Move fill to fastcode see CRT/spiflash_icesugar.ld
4. Implement some of LIBFEMTOGL = DONE
5. Allow print to LCD
6. Why does TypeFaceNum in showChar hang??
*/

#define ST77XX_SWRESET 0x01
#define ST77XX_SLPOUT 0x11
#define ST77XX_NORON 0x13
#define ST77XX_INVOFF 0x20
#define ST77XX_INVON 0x21
#define ST77XX_DISPOFF 0x28
#define ST77XX_DISPON 0x29
#define ST77XX_CASET 0x2A
#define ST77XX_RASET 0x2B
#define ST77XX_RAMWR 0x2C
#define ST77XX_MADCTL 0x36
#define ST77XX_COLMOD 0x3A

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5

#define ST77XX_MADCTL_MY 0x80
#define ST77XX_MADCTL_MX 0x40
#define ST77XX_MADCTL_MV 0x20
#define ST77XX_MADCTL_ML 0x10
#define ST77XX_MADCTL_RGB 0x00

// Some ready-made 16-bit ('565') color settings:
#define ST77XX_BLACK 0x0000
#define ST77XX_WHITE 0xFFFF
#define ST77XX_RED 0xF800
#define ST77XX_GREEN 0x07E0
#define ST77XX_BLUE 0x001F
#define ST77XX_CYAN 0x07FF
#define ST77XX_MAGENTA 0xF81F
#define ST77XX_YELLOW 0xFFE0
#define ST77XX_ORANGE 0xFC00

void progress_led(uint8_t i)
{
  if (LED_PROGRESS)
    reg_gpio_led = i;
}

void delay_ms(uint32_t ms)
{
  // if NOP = 1 cycle then a NOP = 1/12000000 = 83ns = 0.083ms
  for (int i = 0; i < ms * 24; i++)
    asm volatile("");
}

void reset()
{
  reg_lcd_cntl = 0b1; // RST
  delay_ms(250);
  reg_lcd_cntl = 0b11; //
  delay_ms(250);
}

void lcd_init()
{
  print("LCD init....");
  progress_led(0b0);
  reset();
  progress_led(0b1);

  send_cmd(ST77XX_SWRESET); // Software reset
  delay_ms(150);
  progress_led(0b11);

  send_cmd(ST77XX_SLPOUT); // Out of sleep mode
  delay_ms(255);
  progress_led(0b111);

  // send_cmd(ST7735_FRMCTR1); // Framerate control, normal mode: Rate = fosc/(1x2+40) * (LINE+2C+2D)
  // send_data(0x01);
  // send_data(0x2C);
  // send_data(0x2D);

  // send_cmd(ST7735_FRMCTR2); //  4: Framerate ctrl - idle mode, 3 args: Rate = fosc/(1x2+40) * (LINE+2C+2D)
  // send_data(0x01);
  // send_data(0x2C);
  // send_data(0x2D);

  // send_cmd(ST7735_FRMCTR3); //  5: Framerate - partial mode, 6 args:
  // send_data(0x01);
  // send_data(0x2C);
  // send_data(0x2D); //     Dot inversion mode
  // send_data(0x01);
  // send_data(0x2C);
  // send_data(0x2D); //     Line inversion mode

  send_cmd(ST7735_INVCTR); //  6: Display inversion ctrl, 1 arg:
  send_data(0x07);         //     No inversion

  send_cmd(ST7735_PWCTR1); //  7: Power control, 3 args, no delay:
  send_data(0xA2);
  send_data(0x02); //     -4.6V
  send_data(0x84); //     AUTO mode

  send_cmd(ST7735_PWCTR2); //  8: Power control, 1 arg, no delay:
  send_data(0xC5);         //     VGH25=2.4C VGSEL=-10 VGH=3 * AVDD

  send_cmd(ST7735_PWCTR3); // 9: Power control, 2 args, no delay:
  send_data(0x0A);         //     Opamp current small
  send_data(0x00);         //     Boost frequency

  send_cmd(ST7735_PWCTR4); // 10: Power control, 2 args, no delay:
  send_data(0x8A);         //     BCLK/2,
  send_data(0x2A);         //     opamp current small & medium low

  send_cmd(ST7735_PWCTR5); // 11: Power control, 2 args, no delay:
  send_data(0x8A);
  send_data(0xEE);

  send_cmd(ST7735_VMCTR1); // 12: Power control, 1 arg, no delay:
  send_data(0x0E);

  send_cmd(ST77XX_INVON); // 13: Don't invert display, no args

  send_cmd(ST77XX_MADCTL); // 14: Mem access ctl (directions), 1 arg:
  // send_data(0x60);         //     row/col addr, top-bottom refresh
  // send_data(0x08);         //     row/col addr, top-bottom refresh
  // send_data(0xC8); //     row/col addr, top-bottom refresh
  send_data(0x78); //     row/col addr, top-bottom refresh

  send_cmd(ST77XX_CASET);
  send_data(0x00);
  send_data(0x01); // 1
  send_data(0x00);
  send_data(0xA0); // 160

  send_cmd(ST77XX_RASET);
  send_data(0x00);
  send_data(0x1A); // 26
  send_data(0x00);
  send_data(0x69); // 105

  send_cmd(ST77XX_COLMOD); // 15: set color mode, 1 arg, no delay:
  send_data(0x05);         //     16-bit color

  send_cmd(ST77XX_NORON);
  delay_ms(10);

  send_cmd(ST77XX_DISPON);
  delay_ms(100);

  send_cmd(ST77XX_RAMWR);
  progress_led(0b1111);
  print("Done\n");

  lcd_fill(0, 0, 160, 80, ST77XX_BLACK);
  progress_led(0b11111);
}

uint8_t lcd_rgb(uint8_t r, uint8_t g, uint8_t b)
{
  return (b & 0xe0) | ((g >> 3) & 0x1c) | (r >> 6);
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  send_cmd(ST77XX_CASET);
  send_data16(x1 + 1);
  send_data16(x2 + 1);

  send_cmd(ST77XX_RASET);
  send_data16(y1 + 26);
  send_data16(y2 + 26);

  send_cmd(ST77XX_RAMWR);
}

void lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  lcd_address_set(x1, y1, x2 - 1, y2 - 1);
  uint16_t i, j;
  for (i = y1; i < y2; i++)
  {
    for (j = x1; j < x2; j++)
    {
      send_data16(col);
    }
  }
}

void lcd_drawPoint(uint16_t x, uint16_t y, uint16_t c)
{
  lcd_address_set(x, y, x, y);
  send_data16(c);
}

void lcd_drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c)
{
  uint16_t t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1;
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0)
    incx = 1;
  else if (delta_x == 0)
    incx = 0;
  else
  {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0;
  else
  {
    incy = -1;
    delta_y = -delta_y;
  }
  if (delta_x > delta_y)
    distance = delta_x;
  else
    distance = delta_y;
  for (t = 0; t < distance + 1; t++)
  {
    lcd_drawPoint(uRow, uCol, c);
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance)
    {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance)
    {
      yerr -= distance;
      uCol += incy;
    }
  }
}

void lcd_drawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  lcd_drawLine(x1, y1, x2, y1, color);
  lcd_drawLine(x1, y1, x1, y2, color);
  lcd_drawLine(x1, y2, x2, y2, color);
  lcd_drawLine(x2, y1, x2, y2, color);
}

void lcd_showChar(uint16_t x, uint16_t y, uint8_t num, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
  print_dec(sizey);
  uint8_t temp,
      sizex, t, m = 0;
  uint16_t i, TypefaceNum;
  uint16_t x0 = x;
  sizex = sizey / 2;
  TypefaceNum = 16; //(sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
  num = num - 32;
  lcd_address_set(x, y, x + sizex - 1, y + sizey - 1);
  for (i = 0; i < TypefaceNum; i++)
  {
    if (sizey == 12)
      temp = ascii_1206[num][i];
    else if (sizey == 16)
      temp = ascii_1608[num][i];
    else if (sizey == 24)
      temp = ascii_2412[num][i];
    else if (sizey == 32)
      temp = ascii_3216[num][i];
    else
      return;
    for (t = 0; t < 8; t++)
    {
      if (!mode)
      {
        if (temp & (0x01 << t))
          send_data16(fc);
        else
          send_data16(bc);

        m++;
        // if (m % sizex == 0)
        if (m == sizex)
        {
          m = 0;
          break;
        }
      }
      else
      {
        if (temp & (0x01 << t))
          lcd_drawPoint(x, y, fc);
        x++;
        if ((x - x0) == sizex)
        {
          x = x0;
          y++;
          break;
        }
      }
    }
  }
}

void lcd_showString(uint16_t x, uint16_t y, const uint8_t *p, uint16_t fc, uint16_t bc, uint8_t sizey, uint8_t mode)
{
  while (*p != '\0')
  {
    lcd_showChar(x, y, *p, fc, bc, sizey, mode);
    x += sizey / 2;
    p++;
  }
}

void lcd_test()
{
  lcd_fill(5, 5, 55, 30, ST77XX_RED);
  lcd_fill(5, 55, 55, 70, ST77XX_BLUE);
  lcd_drawRectangle(50, 20, 100, 40, ST77XX_GREEN);
  lcd_drawPoint(100, 70, ST77XX_BLUE);
  lcd_showString(60, 40, "H", ST77XX_WHITE, ST77XX_BLACK, 16, 0);
};