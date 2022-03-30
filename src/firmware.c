/*
 *  PicoSoC - A simple example SoC using PicoRV32
 *
 *  Copyright (C) 2017  Clifford Wolf <clifford@clifford.at>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "mem.h"
#include "led.h"
#include "neopixel.h"
#include "spi.h"
#include "lcd.h"

// Memory Map
// 0x00000000 - 0x00FFFFFF		Internal SRAM
// 0x01000000 - 0x01FFFFFF		External Serial Flash
// 0x02000000 - 0x02000003		SPI Flash Controller Config Register
// 0x02000004 - 0x02000007		UART Clock Divider Register
// 0x02000008 - 0x0200000B		UART Send/Recv Data Register
// 0x03000000 - 0xFFFFFFFF		Memory mapped user peripherals

char getchar_prompt(char *prompt)
{
	int32_t c = -1;

	uint32_t cycles_begin, cycles_now, cycles;
	__asm__ volatile ("rdcycle %0" : "=r"(cycles_begin));

	if (prompt)
		print(prompt);

	while (c == -1) {
		__asm__ volatile ("rdcycle %0" : "=r"(cycles_now));
		cycles = cycles_now - cycles_begin;
		if (cycles > 12000000) {
			if (prompt)
				print(prompt);
			cycles_begin = cycles_now;
			// toggle_led(8);	// Toggle RGB RED
		}
		c = reg_uart_data;
	}

	return c;
}

char getchar()
{
	return getchar_prompt(0);
}

void cmd_echo()
{
	print("Return to menu by sending '!'\n\n");
	char c;
	while ((c = getc()) != '!')
		putc(c);
}

// --------------------------------------------------------

void main()
{
 	setup_uart(115200);
 	print("Booting..\n");

	set_flash_qspi_flag();

	rgb_led(255,0,0); // red
	pmod_led(0b00000000); // all off

	cRGB color;
	color.r = 0;
	color.g = 40;
	color.b = 0;
	set_ws2812(color, 0); // this just to see in simulation
	color.g = 0;

	while (getchar_prompt("Press ENTER to continue..\n") != '\r') { /* wait */ }

	rgb_led(0,255,0); // green
	// pmod_led(0b11111111); // all on

	lcd_init();

	print_banner(1);

	cmd_memtest();
	print("\n");

	cmd_print_spi_state();
	print("\n");
	
	while (1)
	{
		print("\n");

		print("Select an action:\n");
		print("\n");
		print("   [1] Read SPI Flash ID\n");
		print("   [2] Read SPI Config Regs\n");
		print("   [3] Switch to default mode\n");
		print("   [4] Switch to Dual I/O mode\n");
		print("   [5] Switch to Quad I/O mode\n");
		print("   [6] Switch to Quad DDR mode\n");
		print("   [7] Toggle continuous read mode\n");
		print("   [M] Run Memtest\n");
		print("   [S] Print SPI state\n");
		print("   [e] Echo UART\n");
		print("   [l] Set LED register\n");
		print("   [n] Increment Neopixel #1 Blue\n");
		print("   [c] Drawtest\n");
		print("\n");

		for (int rep = 10; rep > 0; rep--)
		{
			print("Command> ");
			char cmd = getchar();
			if (cmd > 32 && cmd < 127)
				putc(cmd);
			print("\n");

			switch (cmd)
			{
			case '1':
				cmd_read_flash_id();
				break;
			case '2':
				cmd_read_flash_regs();
				break;
			case '3':
				set_flash_mode_spi();
				break;
			case '4':
				set_flash_mode_dual();
				break;
			case '5':
				set_flash_mode_quad();
				break;
			case '6':
				set_flash_mode_qddr();
				break;
			case '7':
				reg_spictrl = reg_spictrl ^ 0x00100000;
				break;
			case 'M':
				cmd_memtest();
				break;
			case 'S':
				cmd_print_spi_state();
				break;
			case 'e':
				cmd_echo();
				break;
			case 'l':
				cmd_led();
				break;
			case 'n':
				color.b++;
				set_ws2812(color, 1);
				break;
			case 'c':
				lcd_test();
				break;
			default:
				continue;
			}

			break;
		}
	}
}
