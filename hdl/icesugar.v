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

`ifdef PICOSOC_V
`error "icesugar.v must be read before picosoc.v!"
`endif

`define PICOSOC_MEM ice40up5k_spram

module icesugar (
	input CLK,

	output UART_TX,
	input  UART_RX,

	output P2_1,
	output P2_2,
	output P2_3,
	output P2_4,
	output P2_5,
	output P2_6,
	output P2_7,
	output P2_8,

	output neopixel_pin,

	output LED_R,
	output LED_G,
	output LED_B,

	output flash_csb,
	output flash_clk,
	inout  flash_io0,
	inout  flash_io1,
	inout  flash_io2,
	inout  flash_io3,

	inout LCD_SPI_SCL,
	inout LCD_SPI_SDA,
	inout LCD_SPI_RES,
	inout LCD_SPI_DC,
	inout LCD_SPI_CS,
);
	parameter integer MEM_WORDS = 32768;

	assign P2_1 = ~gpio_led_pmod[0];
	assign P2_2 = ~gpio_led_pmod[1];
	assign P2_3 = ~gpio_led_pmod[2];
	assign P2_4 = ~gpio_led_pmod[3];
	assign P2_5 = ~gpio_led_pmod[4];
	assign P2_6 = ~gpio_led_pmod[5];
	assign P2_7 = ~gpio_led_pmod[6];
	assign P2_8 = ~gpio_led_pmod[7];

	reg [5:0] reset_cnt = 0;
	wire resetn = &reset_cnt;

	always @(posedge CLK) begin
		reset_cnt <= reset_cnt + !resetn;
	end

	wire flash_io0_oe, flash_io0_do, flash_io0_di;
	wire flash_io1_oe, flash_io1_do, flash_io1_di;
	wire flash_io2_oe, flash_io2_do, flash_io2_di;
	wire flash_io3_oe, flash_io3_do, flash_io3_di;

	SB_IO #(
		.PIN_TYPE(6'b 1010_01),
		.PULLUP(1'b 0)
	) flash_io_buf [3:0] (
		.PACKAGE_PIN({flash_io3, flash_io2, flash_io1, flash_io0}),
		.OUTPUT_ENABLE({flash_io3_oe, flash_io2_oe, flash_io1_oe, flash_io0_oe}),
		.D_OUT_0({flash_io3_do, flash_io2_do, flash_io1_do, flash_io0_do}),
		.D_IN_0({flash_io3_di, flash_io2_di, flash_io1_di, flash_io0_di})
	);

	// Peripheral BUS

	wire        iomem_valid; // set by picosoc
	reg         iomem_ready; // true if the addressed device is ready

	wire [3:0]  iomem_wstrb;
	wire [31:0] iomem_addr;
	wire [31:0] iomem_wdata;
	reg  [31:0] iomem_rdata; // read from the addressed device

	// wire leds_en     = (iomem_addr[31:24] == 8'h03);
	// wire ws2812_en   = (iomem_addr[31:24] == 8'h05);
	// wire st7735_en   = (iomem_addr[31:24] == 8'h07);

	// wire leds_iomem_ready;
	// leds leds_inst(
	// 	.clk(CLK),
	// 	.reset(resentn),
	// 	.iomem_valid(iomem_valid && leds_en),
	// 	.iomem_wstrb(iomem_wstrb),
	// 	.iomem_addr(iomem_addr),
	// 	.iomem_wdata(iomem_wdata),
	// 	.iomem_ready(leds_iomem_ready)
	// )

	// wire ws2812_iomem_ready;
	// ws2812 #(.NUM_LEDS(7)) ws2812_inst(
	// 	.clk(CLK),
	// 	.reset(!resentn),
	// 	.iomem_valid(iomem_valid && ws2812_en),
	// 	.iomem_wstrb(iomem_wstrb),
	// 	.iomem_addr(iomem_addr),
	// 	.iomem_wdata(iomem_wdata),
	// 	.iomem_ready(ws2812_iomem_ready)
	// );

	// wire st7735_iomem_ready;
	// st7735 st7735_inst(
	// 	.clk(CLK),
	// 	.reset(resentn),
	// 	.iomem_valid(iomem_valid && st7735_en),
	// 	.iomem_wstrb(iomem_wstrb),
	// 	.iomem_addr(iomem_addr),
	// 	.iomem_wdata(iomem_wdata),
	// 	.iomem_ready(st7735_iomem_ready)
	// )

	// pmod and rgb Leds peripheral
	wire [7:0] gpio_led_pmod;
	wire [7:0] gpio_led_r;
	wire [7:0] gpio_led_g;
	wire [7:0] gpio_led_b;
	pwm pwm_r (.clk(CLK), .pwm(gpio_led_r), .out(LED_R));
	pwm pwm_g (.clk(CLK), .pwm(gpio_led_g), .out(LED_G));
	pwm pwm_b (.clk(CLK), .pwm(gpio_led_b), .out(LED_B));

	// neopixel peripheral
	reg led_write = 0;
	reg [7:0] led_num = 0;
	reg [23:0] led_rgb_data = 0;
	ws2812 #(.NUM_LEDS(7)) ws2812_inst(.data(neopixel_pin), .clk(CLK), .reset(!resetn), .rgb_data(led_rgb_data), .led_num(led_num), .write(led_write));

	// spi st7735 peripheral
	reg lcd_spi_wr;
	reg lcd_spi_ready;
	spi st7735_inst(
		.clk(clk),
		.resetn(resetn),
		.ctrl_wr(lcd_spi_wr),
		.ctrl_addr(iomem_addr[7:0]),
		.ctrl_wdat(iomem_wdata),
		.ctrl_done(lcd_spi_ready),
		.mosi(LCD_SPI_SDA),
		.sclk(LCD_SPI_SCL),
		.cs(LCD_SPI_CS),
		.dc(LCD_SPI_DC),
		.rst(LCD_SPI_RES)
	);

	always @(posedge CLK) begin
		led_write <= 0;
		lcd_spi_wr <= 0;
		if (!resetn) begin
			gpio_led_pmod <= 0;
			gpio_led_r <= 0;
			gpio_led_g <= 0;
			gpio_led_b <= 0;
			led_write < = 0;
			led_num < = 0;
			led_rgb_data = 0;
		end else begin
			iomem_ready <= 0;
			if (iomem_valid && !iomem_ready && iomem_addr[31:24] == 8'h 03) begin
				iomem_ready <= 1;
				iomem_rdata[ 7: 0] <= gpio_led_pmod[7:0];
				iomem_rdata[15: 8] <= gpio_led_r[7:0];
				iomem_rdata[23:16] <= gpio_led_g[7:0];
				iomem_rdata[31:24] <= gpio_led_b[7:0];

				if (iomem_wstrb[0]) gpio_led_pmod[7:0] <= iomem_wdata[ 7: 0];
				if (iomem_wstrb[1]) gpio_led_r   [7:0] <= iomem_wdata[15: 8];
				if (iomem_wstrb[2]) gpio_led_g   [7:0] <= iomem_wdata[23:16];
				if (iomem_wstrb[3]) gpio_led_b   [7:0] <= iomem_wdata[31:24];
			end
			else if (iomem_valid && !iomem_ready && iomem_addr[31:24] == 8'h04) begin
				iomem_ready <= 1;
        led_write <= 1;
				iomem_rdata <= 0;
				if (iomem_wstrb[0]) led_num             <= iomem_wdata[ 7: 0];
				if (iomem_wstrb[1]) led_rgb_data[ 7: 0] <= iomem_wdata[15: 8];
				if (iomem_wstrb[2]) led_rgb_data[15: 8] <= iomem_wdata[23:16];
				if (iomem_wstrb[3]) led_rgb_data[23:16] <= iomem_wdata[31:24];
			end
			else if (iomem_valid && !iomem_ready && iomem_addr[31:24] == 8'h05) begin
				iomem_ready = lcd_spi_ready;
				lcd_spi_wr <= lcd_spi_ready ? 0 : iomem_wstrb;
			end
		end
	end

	picosoc #(
		.BARREL_SHIFTER(0),
		.ENABLE_MULDIV(0),
		.MEM_WORDS(MEM_WORDS)
	) soc (
		.clk          (CLK         ),
		.resetn       (resetn      ),

		.ser_tx       (UART_TX     ),
		.ser_rx       (UART_RX     ),

		.flash_csb    (flash_csb   ),
		.flash_clk    (flash_clk   ),

		.flash_io0_oe (flash_io0_oe),
		.flash_io1_oe (flash_io1_oe),
		.flash_io2_oe (flash_io2_oe),
		.flash_io3_oe (flash_io3_oe),

		.flash_io0_do (flash_io0_do),
		.flash_io1_do (flash_io1_do),
		.flash_io2_do (flash_io2_do),
		.flash_io3_do (flash_io3_do),

		.flash_io0_di (flash_io0_di),
		.flash_io1_di (flash_io1_di),
		.flash_io2_di (flash_io2_di),
		.flash_io3_di (flash_io3_di),

		.irq_5        (1'b0        ),
		.irq_6        (1'b0        ),
		.irq_7        (1'b0        ),

		.iomem_valid  (iomem_valid ),
		.iomem_ready  (iomem_ready ),
		.iomem_wstrb  (iomem_wstrb ),
		.iomem_addr   (iomem_addr  ),
		.iomem_wdata  (iomem_wdata ),
		.iomem_rdata  (iomem_rdata )
	);
endmodule
