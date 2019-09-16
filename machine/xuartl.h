// See LICENSE for license details.

#ifndef _XUARTL_H
#define _XUARTL_H

#include <stdint.h>

extern volatile uint32_t* xuartl;

//==Register Space
#define XUARTL_RX_FIFO 0
#define XUARTL_TX_FIFO 1
#define XUARTL_STAT_REG 2
#define XUARTL_CTRL_REG 3

//==Bits of XUARTL_CTRL_REG
#define XUARTL_RST_TXFIFO 0b00001
#define XUARTL_RST_RXFIFO 0b00010
#define XUARTL_ENABLE_INTR 0b10000

//==Bits of XUARTL_STAT_REG
#define XUARTL_RXFIFO_VALID  0b00000001
#define XUARTL_RXFIFO_FULL   0b00000010
#define XUARTL_TXFIFO_EMPTY  0b00000100
#define XUARTL_TXFIFO_FULL   0b00001000
#define XUARTL_INTR_EN       0b00010000
#define XUARTL_OVERRUN_ERR   0b00100000
#define XUARTL_FRAME_ERR     0b01000000
#define XUARTL_PARITY_ERR    0b10000000

void xuartl_putchar(uint8_t ch);
int xuartl_getchar();
void query_xuartl(uintptr_t dtb);

#endif
