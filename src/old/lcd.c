#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"
#include "../../uart.h"

#define CMD_SWRESET 0x01      // software reset
#define CMD_SLPOUT 0x11       // sleep out
#define CMD_INVCTR 0xb4       // display inversion control
#define CMD_PARAM_INVCTR 0x03 // normal mode
#define CMD_PWCTR1 0xC0
#define CMD_PARAM1_PWCTR1 0x82
#define CMD_PARAM2_PWCTR1 0x02
#define CMD_PARAM3_PWCTR1 0x84
#define CMD_PWCTR4 0xC3
#define CMD_PARAM1_PWCTR4 0x8A
#define CMD_PARAM2_PWCTR4 0x2E
#define CMD_PWCTR5 0xC4
#define CMD_PARAM1_PWCTR5 0x8A
#define CMD_PARAM2_PWCTR5 0xAA
#define CMD_VMCTR1 0xC5
#define CMD_PARAM_VMCTR1 0x0E
#define CMD_INVOFF 0x20
#define CMD_MADCTL 0x36       // Rotation
#define CMD_PARAM_MADCTL 0x68 // 0x08, 0xC8, 0x68
#define CMD_COLMOD 0x3A
#define CMD_PARAM_COLMOD 0x05
#define CMD_CASET 0x2A
#define CMD_PARAM1_CASET 0x00
#define CMD_PARAM2_CASET 0x1A
#define CMD_PARAM3_CASET 0x00
#define CMD_PARAM4_CASET 0x69
#define CMD_RASET 0x2B
#define CMD_PARAM1_RASET 0x00
#define CMD_PARAM2_RASET 0x01
#define CMD_PARAM3_RASET 0x00
#define CMD_PARAM4_RASET 0xA0
#define CMD_NORON 0x13
#define CMD_DISPINV 0x21
#define CMD_DISPON 0x29
#define CMD_RAMWR 0x2C

#define ST7735_TFTWIDTH_80 80    // for mini
#define ST7735_TFTHEIGHT_160 160 // for 1.8" and mini display

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

// void printByteMsg(char *msg, uint32_t v)
// {
//   print(msg);
//   print_hex(v, 8);
//   print("\n");
// }

void write_bus16(uint16_t dat)
{
  lcd_cs(0);
  for (uint8_t i = 0; i < 16; i++)
  {
    lcd_sclk(0);
    lcd_mosi(dat & 0x80 ? 1 : 0);
    lcd_sclk(1);
    dat <<= 1;
  }
  lcd_cs(1);
}

void write_bus(uint8_t dat)
{
  lcd_cs(0);
  for (uint8_t i = 0; i < 8; i++)
  {
    lcd_sclk(0);
    lcd_mosi(dat & 0x80 ? 1 : 0);
    lcd_sclk(1);
    dat <<= 1;
  }
  lcd_cs(1);
}

void send_data(uint8_t dat)
{
  write_bus(dat);
}

void send_data16(uint16_t dat)
{
  // write_bus(dat >> 8);
  // write_bus(dat);
  write_bus16(dat);
}

void send_cmd(uint8_t dat)
{
  lcd_dc(0);
  write_bus(dat);
  lcd_dc(1);
}

void delay_ms(uint32_t ms)
{
  // if NOP = 1 cycle then a NOP = 1/12000000 = 83ns = 0.083ms
  for (int i = 0; i < ms * 24; i++)
    asm volatile("");
}

void reset()
{
  lcd_rst(0);
  delay_ms(100);
  lcd_rst(1);
  delay_ms(100);
}

/*void lcd_init()
{
  print("Initialising st7735\n");

  reset();

  send_cmd(CMD_SWRESET);
  delay_ms(150);

  send_cmd(CMD_SLPOUT);
  delay_ms(500);

  send_cmd(CMD_INVCTR);
  send_data(CMD_PARAM_INVCTR);

  send_cmd(CMD_PWCTR1);
  send_data(CMD_PARAM1_PWCTR1);
  send_data(CMD_PARAM2_PWCTR1);
  send_data(CMD_PARAM3_PWCTR1);

  send_cmd(CMD_PWCTR4);
  send_data(CMD_PARAM1_PWCTR4);
  send_data(CMD_PARAM2_PWCTR4);

  send_cmd(CMD_PWCTR5);
  send_data(CMD_PARAM1_PWCTR4);
  send_data(CMD_PARAM2_PWCTR4);

  send_cmd(CMD_VMCTR1);
  send_data(CMD_PARAM_VMCTR1);

  send_cmd(CMD_DISPINV);

  send_cmd(CMD_MADCTL);
  send_data(CMD_PARAM_MADCTL);

  send_cmd(CMD_COLMOD);
  send_data(CMD_PARAM_COLMOD);

  send_cmd(CMD_CASET);
  send_data(CMD_PARAM1_CASET);
  send_data(CMD_PARAM2_CASET);
  send_data(CMD_PARAM3_CASET);
  send_data(CMD_PARAM4_CASET);

  send_cmd(CMD_RASET);
  send_data(CMD_PARAM1_RASET);
  send_data(CMD_PARAM2_RASET);
  send_data(CMD_PARAM3_RASET);
  send_data(CMD_PARAM4_RASET);

  send_cmd(CMD_NORON);
  delay_ms(10);

  send_cmd(CMD_DISPON);
  delay_ms(100);

  send_cmd(CMD_RAMWR);

  lcd_fill(0, 0, 160, 80, 0x0000);
}*/

void lcd_init()
{
  reset();

  send_cmd(ST77XX_SWRESET); // Software reset
  delay_ms(150);

  send_cmd(ST77XX_SLPOUT); // Out of sleep mode
  delay_ms(255);

  send_cmd(ST7735_FRMCTR1); // Framerate control, normal mode: Rate = fosc/(1x2+40) * (LINE+2C+2D)
  send_data(0x01);
  send_data(0x2C);
  send_data(0x2D);

  send_cmd(ST7735_FRMCTR2); //  4: Framerate ctrl - idle mode, 3 args: Rate = fosc/(1x2+40) * (LINE+2C+2D)
  send_data(0x01);
  send_data(0x2C);
  send_data(0x2D);

  send_cmd(ST7735_FRMCTR3); //  5: Framerate - partial mode, 6 args:
  send_data(0x01);
  send_data(0x2C);
  send_data(0x2D); //     Dot inversion mode
  send_data(0x01);
  send_data(0x2C);
  send_data(0x2D); //     Line inversion mode

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
  // send_data(0xC8);         //     row/col addr, top-bottom refresh
  send_data(0x60); //     row/col addr, top-bottom refresh
  // send_data(0xA0); //     row/col addr, bottom-top refresh

  send_cmd(ST77XX_CASET);
  send_data(0x00);
  send_data(0x01); // 1
  send_data(0x00);
  send_data(0xA0); // 160

  send_cmd(ST77XX_RASET);
  send_data(0x00);
  send_data(0x1A); // 24
  send_data(0x00);
  send_data(0x69); // 103

  send_cmd(ST77XX_COLMOD); // 15: set color mode, 1 arg, no delay:
  send_data(0x05);         //     16-bit color

  send_cmd(0xe0);
  send_data(0x02);
  send_data(0x1c);
  send_data(0x07);
  send_data(0x12);
  send_data(0x37);
  send_data(0x32);
  send_data(0x29);
  send_data(0x2d);
  send_data(0x29);
  send_data(0x25);
  send_data(0x2B);
  send_data(0x39);
  send_data(0x00);
  send_data(0x01);
  send_data(0x03);
  send_data(0x10);

  send_cmd(0xe1);
  send_data(0x03);
  send_data(0x1d);
  send_data(0x07);
  send_data(0x06);
  send_data(0x2E);
  send_data(0x2C);
  send_data(0x29);
  send_data(0x2D);
  send_data(0x2E);
  send_data(0x2E);
  send_data(0x37);
  send_data(0x3F);
  send_data(0x00);
  send_data(0x00);
  send_data(0x02);
  send_data(0x10);

  send_cmd(ST77XX_NORON);
  delay_ms(10);

  send_cmd(ST77XX_DISPON);
  delay_ms(100);

  send_cmd(ST77XX_RAMWR);

  // lcd_fill(0, 0, 160, 80, 0b0000011111100000);
}

uint8_t lcd_rgb(uint8_t r, uint8_t g, uint8_t b)
{
  return (r & 0xe0) | ((g >> 3) & 0x1c) | (b >> 6);
}

void lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col)
{
  lcd_address_set(x1, y1, x2, y2);
  uint16_t i, j;
  for (i = y1; i < y2; i++)
  {
    for (j = x1; j < x2; j++)
    {
      send_data16(col);
    }
  }
}

void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  // send_cmd(CMD_CASET);
  // send_data(x1 + 1);
  // send_data(x2 + 1);
  // send_cmd(CMD_RASET);
  // send_data(y1 + 26);
  // send_data(y2 + 26);
  // send_cmd(CMD_RAMWR);
}

void lcd_test()
{
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
  send_data16(0);
};