#ifndef LCD_H
#define LCD_H

#define USE_HORIZONTAL 2
#define LCD_WIDTH 160
#define LCD_HEIGHT 80

static inline void lcd_sclk(uint32_t bitmask) {
  *(volatile uint32_t*)(0x0500000c) = bitmask;
}

static inline void lcd_cs(uint32_t bitmask) {
    *(volatile uint32_t*)(0x05000004) = bitmask;
}

static inline uint32_t lcd_getcs() {
    return *(volatile uint32_t*)(0x05000004);
}

static inline void lcd_dc(uint32_t bitmask) {
    *(volatile uint32_t*)(0x05000010) = bitmask;
}

static inline uint32_t lcd_getdc() {
    return *(volatile uint32_t*)(0x05000010);
}

static inline void lcd_rst(uint32_t bitmask) {
    *(volatile uint32_t*)(0x05000014) = bitmask;
}

static inline uint32_t lcd_getrst() {
    return *(volatile uint32_t*)(0x05000014);
}

static inline void lcd_mosi(uint8_t value) {
    *(volatile uint32_t*)(0x05000008) = value;
}

// static inline uint8_t lcd_xfer(uint8_t value) {
//     *(volatile uint32_t*)(0x05000008) = value;
//     return *(volatile uint32_t*)(0x05000008);
// }

// static inline void lcd_prescale(uint32_t value) {
//     *(volatile uint32_t*)(0x05000000) = value;
// }

// static inline void lcd_mode(bool cpol, bool cpha) {
//     *(volatile uint32_t*)(0x0500000c) = (cpol ? 1 : 0) | (cpha ? 2 : 0);
// }

void lcd_init();
void lcd_fillred();
void lcd_test();

#endif
