#ifndef SPI_H
#define SPI_H

#define reg_spictrl     (*(volatile uint32_t*)0x02000000)

void set_flash_qspi_flag();
void cmd_print_spi_state();
void set_flash_mode_spi();
void set_flash_mode_dual();
void set_flash_mode_quad();
void set_flash_mode_qddr();
void enable_flash_crm();
void cmd_read_flash_id();
uint8_t cmd_read_flash_reg(uint8_t cmd);
void cmd_read_flash_regs();

#endif