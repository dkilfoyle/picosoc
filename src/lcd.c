#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"
#include "uart.h"

#define CMD_SWRESET 0x01 //software reset
#define CMD_SLPOUT 0x11 //sleep out
#define CMD_INVCTR 0xb4 //display inversion control
#define CMD_PARAM_INVCTR 0x03 //normal mode
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
#define CMD_MADCTL 0x36
#define CMD_PARAM_MADCTL 0xC8
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


void printByteMsg(char* msg, uint32_t v) {
  print(msg);
  print_hex(v, 8);
  print("\n");
}

// void send_cmd(uint8_t r) {
//   print("send_cmd\n");
// 	lcd_cs(1);
// 	lcd_dc(0);
// 	lcd_cs(0);

//   print("xfering\n");
// 	uint8_t d = lcd_xfer(r);

//   print("xfering done\n");

// 	lcd_cs(1);
// 	printByteMsg("Command", d);
// }

// void send_data(uint8_t d) {
// 	lcd_cs(1);
// 	lcd_dc(1);
// 	lcd_cs(0);

// 	uint8_t r2 = lcd_xfer(d);

// 	lcd_cs(1);
//   printByteMsg("Data", r2);
// }

// void send_data16(uint16_t d) {
//   send_data(d>>8);
//   send_data(d);
// }

void write_bus(uint8_t dat) {
  lcd_cs(0);
  for (uint8_t i=0; i<8; i++) {
    lcd_sclk(0);
    lcd_mosi(dat & 0x80 ? 1 : 0);
    lcd_sclk(1);
    dat<<=1;
  }
  lcd_cs(1);
}

void send_data(uint8_t dat) {
  write_bus(dat);
}

void send_data16(uint16_t dat) {
  write_bus(dat >> 8);
  write_bus(dat);
}

void send_cmd(uint8_t dat) {
  lcd_dc(0);
  write_bus(dat);
  lcd_dc(1);
}

void delay_ms(uint32_t ms) {
  // if NOP = 1 cycle then a NOP = 1/12000000 = 83ns = 0.083ms
  for (int i=0; i < ms*24; i++) asm volatile ("");
}

void reset() {
  lcd_rst(0);
  delay_ms(100);
  lcd_rst(1);
  delay_ms(100);
}

void lcd_init() {
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

  lcd_test();



}

void lcd_init2() {
  print("Initialising st7735\n");
  // lcd_cs(1);
  // lcd_rst(0);
  // lcd_prescale(4);
  // lcd_mode(true, true);

  print("Reset st7735\n");
  reset();

  print("Finished reset\n");

	// LCD_RES_Clr();//��λ
	// delay_ms(100);
	// LCD_RES_Set();
	// delay_ms(100);
	
	// LCD_BLK_Set();//�򿪱���
  // delay_ms(100);
	
	send_cmd(0x11);     //Sleep out
	delay_ms(120);      //Delay 120ms

	send_cmd(0xB1);     //Normal mode
	send_data(0x05);   
	send_data(0x3C);   
	send_data(0x3C); 

	send_cmd(0xB2);     //Idle mode
	send_data(0x05);   
	send_data(0x3C);   
	send_data(0x3C);   
	
  send_cmd(0xB3);     //Partial mode
	send_data(0x05);   
	send_data(0x3C);   
	send_data(0x3C);   
	send_data(0x05);   
	send_data(0x3C);   
	send_data(0x3C);   
	
  send_cmd(0xB4);     //Dot inversion
	send_data(0x03);   
	
  send_cmd(0xC0);     //AVDD GVDD
	send_data(0xAB);   
	send_data(0x0B);   
	send_data(0x04);   
	
  send_cmd(0xC1);     //VGH VGL
	send_data(0xC5);   //C0
	
  send_cmd(0xC2);     //Normal Mode
	send_data(0x0D);   
	send_data(0x00);   
	
  send_cmd(0xC3);     //Idle
	send_data(0x8D);   
	send_data(0x6A);   
	
  send_cmd(0xC4);     //Partial+Full
	send_data(0x8D);   
	send_data(0xEE);   
	
  send_cmd(0xC5);     //VCOM
	send_data(0x0F);   
	
  // send_cmd(0xE0);     //positive gamma
	// send_data(0x07);   
	// send_data(0x0E);   
	// send_data(0x08);   
	// send_data(0x07);   
	// send_data(0x10);   
	// send_data(0x07);   
	// send_data(0x02);   
	// send_data(0x07);   
	// send_data(0x09);   
	// send_data(0x0F);   
	// send_data(0x25);   
	// send_data(0x36);   
	// send_data(0x00);   
	// send_data(0x08);   
	// send_data(0x04);   
	// send_data(0x10);   
	
  // send_cmd(0xE1);     //negative gamma
	// send_data(0x0A);   
	// send_data(0x0D);   
	// send_data(0x08);   
	// send_data(0x07);   
	// send_data(0x0F);   
	// send_data(0x07);   
	// send_data(0x02);   
	// send_data(0x07);   
	// send_data(0x09);   
	// send_data(0x0F);   
	// send_data(0x25);   
	// send_data(0x35);   
	// send_data(0x00);   
	// send_data(0x09);   
	// send_data(0x04);   
	// send_data(0x10);
		 
	send_cmd(0xFC);    
	send_data(0x80);  
		
	send_cmd(0x3A);     // Color mode
	send_data(0x05);    // 16 bit

	send_cmd(0x36);
	if(USE_HORIZONTAL==0)send_data(0x08);
	else if(USE_HORIZONTAL==1)send_data(0xC8);
	else if(USE_HORIZONTAL==2)send_data(0x78);
	else send_data(0xA8);   

	send_cmd(0x21);     //Display inversion
	send_cmd(0x29);     //Display on

	send_cmd(0x2A);     //Set Column Address
	send_data(0x00);   
	send_data(0x1A);  //26  
	send_data(0x00);   
	send_data(0x69);   //105 

	send_cmd(0x2B);     //Set Page Address
	send_data(0x00);   
	send_data(0x01);    //1
	send_data(0x00);   
	send_data(0x50);    //160
	// send_data(0xA0);    //160

	send_cmd(0x2C);     // RAM Write

  print("Finished init\n");
  lcd_test();

}

void lcd_test() {
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b1111100000000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000011111000000);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
  send_data16(0b0000000000111111);
}