#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "icosoc.h"
#include "gfx.h"
//isococ example for SPI1306 - need to adapt for st7735


static inline void icosoc_@name@_cs(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000004 + @addr@ * 0x10000) = bitmask;
}
static inline uint32_t icosoc_@name@_getcs() {
    return *(volatile uint32_t*)(0x20000004 + @addr@ * 0x10000);
}
static inline void icosoc_@name@_dc(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000010 + @addr@ * 0x10000) = bitmask;
}
static inline uint32_t icosoc_@name@_getdc() {
    return *(volatile uint32_t*)(0x20000010 + @addr@ * 0x10000);
}
static inline void icosoc_@name@_rst(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000014 + @addr@ * 0x10000) = bitmask;
}
static inline uint32_t icosoc_@name@_getrst() {
    return *(volatile uint32_t*)(0x20000014 + @addr@ * 0x10000);
}
static inline uint8_t icosoc_@name@_xfer(uint8_t value) {
    *(volatile uint32_t*)(0x20000008 + @addr@ * 0x10000) = value;
    return *(volatile uint32_t*)(0x20000008 + @addr@ * 0x10000);
}
static inline void icosoc_@name@_prescale(uint32_t value) {
    *(volatile uint32_t*)(0x20000000 + @addr@ * 0x10000) = value;
}
static inline void icosoc_@name@_mode(bool cpol, bool cpha) {
    *(volatile uint32_t*)(0x2000000c + @addr@ * 0x10000) = (cpol ? 1 : 0) | (cpha ? 2 : 0);
}

void send_cmd(uint8_t r) {
	icosoc_oled_cs(1);
	icosoc_oled_dc(0);
	icosoc_oled_cs(0);

	uint8_t d = icosoc_oled_xfer(r);
	icosoc_oled_cs(1);
	//printf("Command %x\n", d);
}

void send_data(uint8_t d) {
	icosoc_oled_cs(1);
	icosoc_oled_dc(1);
	icosoc_oled_cs(0);

	uint8_t r2 = icosoc_oled_xfer(d);

	icosoc_oled_cs(1);
	//printf("Data %x\n", r);
}

void display() {
	//send_cmd(0x21);
	//send_cmd(0x00);
	//send_cmd(0x7f);
	//send_cmd(0x22);
	//send_cmd(0x00);
	//send_cmd(0x07);

	for(int i=0;i<8;i++) {
		send_cmd(0xb0 + i);
		send_cmd(0x02);
		send_cmd(0x10);
		for(int j=0;j<128;j++) {
			send_data(buffer[i*128 + j]);
		}
	}
}

void reset() {
	icosoc_oled_rst(1);
        for (int i = 0; i < 20000; i++) asm volatile ("");
	icosoc_oled_rst(0);
        for (int i = 0; i < 200000; i++) asm volatile ("");
	icosoc_oled_rst(1);
}

void init() {
	printf("Initialising\n");
	icosoc_oled_cs(1);
	icosoc_oled_rst(1);
	icosoc_oled_prescale(4); // 5Mhz
	icosoc_oled_mode(false, false);
	printf("Reset\n");
	reset();
        printf("Reset pin is %ld\n", icosoc_oled_getrst());
	send_cmd(0xAE); // Display off
	send_cmd(0xA6); // Normal display
	send_cmd(0x02); // Set low column 2
	send_cmd(0x40); // Set start line zero
	send_cmd(0xC8); // Com scan Dec
	send_cmd(0x81); // Set contrast
	send_cmd(0xCF); // Set contrast
	//send_cmd(0x9F); // Set contrast
	send_cmd(0xA4); // Set Display all on resume
	send_cmd(0xA8); // Set multiplex
	send_cmd(0x3F); // Set multiplex
	send_cmd(0xD3); // Set Display offset
	send_cmd(0x00); // Set Display offset
	send_cmd(0xD5); // Set Display Clock Div
	send_cmd(0x80); // Set Display Clock Div
	send_cmd(0xD9); // Set precharge
	send_cmd(0xF1); // Set precharge
	//send_cmd(0x22); // Set precharge
	send_cmd(0xDA); // Set Comp Ins
	send_cmd(0x12); // Set Comp Ins
	send_cmd(0xDB); // Set Vcom Detect
	send_cmd(0x40); // Set Vcom Detect
	send_cmd(0x8D); // Charge pump
	send_cmd(0x14); // Charge pump
	//send_cmd(0x10); // Charge pump
	send_cmd(0x20); // Memory mode
	send_cmd(0x00); // Memory mode
	send_cmd(0xA1); // Seg remap 1
	send_cmd(0x2E); // Deactivate scroll.
	send_cmd(0xAF); // Switch on

	printf("Initialisation done\n");

}

int main()
{
	init();
	drawText(20,30,"Hello World!",1);

	for (uint8_t i = 0;; i++)
	{
		icosoc_leds(i);
		display();	

		for (int i = 0; i < 100000; i++)
			asm volatile ("");

	}
}

// actual st7735 code



#include "lcd_init.h"
#include "delay.h"

void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

#if 0
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA
 	GPIO_SetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5);
#endif
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��B�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //��ʼ��GPIOB
 	GPIO_SetBits(GPIOB, GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
}


/******************************************************************************
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  �� 
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	u8 i;
	LCD_CS_Clr();
	for(i=0;i<8;i++)
	{			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	}	
  LCD_CS_Set();	
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//д����
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+26);
		LCD_WR_DATA(x2+26);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//������д
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
	else
	{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1+26);
		LCD_WR_DATA(y2+26);
		LCD_WR_REG(0x2c);//������д
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Init();//��ʼ��GPIO
	
	LCD_RES_Clr();//��λ
	delay_ms(100);
	LCD_RES_Set();
	delay_ms(100);
	
	LCD_BLK_Set();//�򿪱���
  delay_ms(100);
	
	LCD_WR_REG(0x11);     //Sleep out
	delay_ms(120);                //Delay 120ms
	LCD_WR_REG(0xB1);     //Normal mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB2);     //Idle mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB3);     //Partial mode
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x05);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_DATA8(0x3C);   
	LCD_WR_REG(0xB4);     //Dot inversion
	LCD_WR_DATA8(0x03);   
	LCD_WR_REG(0xC0);     //AVDD GVDD
	LCD_WR_DATA8(0xAB);   
	LCD_WR_DATA8(0x0B);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_REG(0xC1);     //VGH VGL
	LCD_WR_DATA8(0xC5);   //C0
	LCD_WR_REG(0xC2);     //Normal Mode
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_REG(0xC3);     //Idle
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0x6A);   
	LCD_WR_REG(0xC4);     //Partial+Full
	LCD_WR_DATA8(0x8D);   
	LCD_WR_DATA8(0xEE);   
	LCD_WR_REG(0xC5);     //VCOM
	LCD_WR_DATA8(0x0F);   
	LCD_WR_REG(0xE0);     //positive gamma
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0E);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x36);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);   
	LCD_WR_REG(0xE1);     //negative gamma
	LCD_WR_DATA8(0x0A);   
	LCD_WR_DATA8(0x0D);   
	LCD_WR_DATA8(0x08);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x02);   
	LCD_WR_DATA8(0x07);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x0F);   
	LCD_WR_DATA8(0x25);   
	LCD_WR_DATA8(0x35);   
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x09);   
	LCD_WR_DATA8(0x04);   
	LCD_WR_DATA8(0x10);
		 
	LCD_WR_REG(0xFC);    
	LCD_WR_DATA8(0x80);  
		
	LCD_WR_REG(0x3A);     
	LCD_WR_DATA8(0x05);   
	LCD_WR_REG(0x36);
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x08);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC8);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x78);
	else LCD_WR_DATA8(0xA8);   
	LCD_WR_REG(0x21);     //Display inversion
	LCD_WR_REG(0x29);     //Display on
	LCD_WR_REG(0x2A);     //Set Column Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x1A);  //26  
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x69);   //105 
	LCD_WR_REG(0x2B);     //Set Page Address
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0x01);    //1
	LCD_WR_DATA8(0x00);   
	LCD_WR_DATA8(0xA0);    //160
	LCD_WR_REG(0x2C); 
}






