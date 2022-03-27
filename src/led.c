#include <stdint.h>

#include "led.h"
#include "uart.h"

/**
 * Set LED PMOD value (PMOD 2)
 */
void pmod_led(int pmod)
{
    uint32_t rgb = reg_gpio_led & 0xFFFFFF00;
    
    reg_gpio_led = rgb | pmod;
}

/**
 * Set RGB LED value
 */
void rgb_led(int r, int g, int b)
{
    uint32_t pmod = reg_gpio_led & 0x000000FF;
    uint32_t rgb = b << 24 | g << 16 | r << 8;
    
    reg_gpio_led = rgb | pmod;
}

void toggle_rgbled(uint8_t pos)
{
    // uint32_t pmod = reg_gpio_led & 0x000000FF;
    // uint32_t rgb = b << 24 | g << 16 | r << 8;
    
    // reg_gpio_led = rgb | pmod;
}

void cmd_led()
{
	print("\nOptions:\n");
	print("  [0] ALL OFF\n");
	print("  [1] ALL ON\n");
	print("  [2] RGB RED\n");
	print("  [3] RGB GREEN\n");
	print("  [4] RGB BLUE\n");
	print("  [5] PMOD2 ON\n");
	print("\n  [0-5] > ");

	char c = getc();
	putc(c);
	putc('\n');
	c -= 48;
	
	if (c < 0 || c > 9)
	{
		print("  Inavlid option\n");
		return;
	}

	switch (c)
	{
	case 0:
        pmod_led(0b00000000);
		break;
	
	case 1:
        pmod_led(0b11111111);
		break;
	
	case 2:
		rgb_led(255,0,0);
		break;
	
	case 3:
		rgb_led(0,255,0);
		break;

	case 4:
        rgb_led(0,0,255);
		break;
	
	default:
		break;
	}

	print("  LED REG: 0x");
	print_hex(reg_gpio_led, 2);
	putc('\n');
}