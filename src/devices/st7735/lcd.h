#ifndef LCD_H
#define LCD_H

#define LCD_WIDTH 160
#define LCD_HEIGHT 80
#define LED_PROGRESS 1

// 1 hot encoded lcd registers
#define reg_lcd_cntl (*(volatile uint32_t *)0x06000004)  // 0b000100
#define reg_lcd_cmd (*(volatile uint32_t *)0x06000008)   // 0b001000
#define reg_lcd_dat (*(volatile uint32_t *)0x06000010)   // 0b010000
#define reg_lcd_dat16 (*(volatile uint32_t *)0x06000020) // 0b100000

#define send_cmd(d8) reg_lcd_cmd = d8
#define send_data(d8) reg_lcd_dat = d8
#define send_data16(d16) reg_lcd_dat16 = d16

void lcd_init();
void lcd_fillred();
void lcd_test();
void lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t col);
void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif
