
#ifndef MEM_H
#define MEM_H

#define MEM_TOTAL 0x20000 /* 128 KB for iCESugar (UP5K) */

uint32_t xorshift32(uint32_t *state);
void     mem_test(int mem_total);
void cmd_memtest();

#endif