// See LICENSE for license details.

#include <string.h>
#include "xuartl.h"
#include "fdt.h"

volatile uint32_t* xuartl;

void xuartl_putchar(uint8_t ch)
{
    while( xuartl[XUARTL_STAT_REG] & XUARTL_TXFIFO_FULL){
       asm("nop");
    }
    xuartl[XUARTL_TX_FIFO] = ch;
/*
#ifdef __riscv_atomic
    int32_t r;
    do {
      __asm__ __volatile__ (
        "amoor.w %0, %2, %1\n"
        : "=r" (r), "+A" (uart[UART_REG_TXFIFO])
        : "r" (ch));
    } while (r < 0);
#else
    volatile uint32_t *tx = uart + UART_REG_TXFIFO;
    while ((int32_t)(*tx) < 0);
    *tx = ch;
#endif
*/
}

int xuartl_getchar()
{
  if (xuartl[XUARTL_STAT_REG] & XUARTL_RXFIFO_VALID) {
    return xuartl[XUARTL_RX_FIFO];
  }
  return -1;
}

struct xuartl_scan
{
  int compat;
  uint64_t reg;
  uint64_t clock;
};

static void xuartl_open(const struct fdt_scan_node *node, void *extra)
{
  struct xuartl_scan *scan = (struct xuartl_scan *)extra;
  memset(scan, 0, sizeof(*scan));
}

static void xuartl_prop(const struct fdt_scan_prop *prop, void *extra)
{
  struct xuartl_scan *scan = (struct xuartl_scan *)extra;
  if (!strcmp(prop->name, "compatible") && !strcmp((const char*)prop->value, "xlnx,axi-uartlite-1.02.a")) {
    scan->compat = 1;
  } else if (!strcmp(prop->name, "reg")) {
    fdt_get_address(prop->node->parent, prop->value, &scan->reg);
  } else if (!strcmp(prop->name, "clock")) {
    fdt_get_address(prop->node->parent, prop->value, &scan->clock);
  }
}

static void xuartl_done(const struct fdt_scan_node *node, void *extra)
{
  uint32_t clock;
  struct xuartl_scan *scan = (struct xuartl_scan *)extra;
  if (!scan->compat || !scan->reg || xuartl) return;

  clock = scan->clock;
  xuartl = (void*)((uintptr_t)scan->reg);

  /*
  Baudrate is set during rtl design
  */
  xuartl[XUARTL_CTRL_REG] = XUARTL_RST_TXFIFO | 
                            XUARTL_RST_RXFIFO;
  xuartl[XUARTL_CTRL_REG] = 0;
}

void query_xuartl(uintptr_t fdt)
{
  struct fdt_cb cb;
  struct xuartl_scan scan;

  memset(&cb, 0, sizeof(cb));
  xuartl = 0;
  cb.open = xuartl_open;
  cb.prop = xuartl_prop;
  cb.done = xuartl_done;
  cb.extra = &scan;

  fdt_scan(fdt, &cb);
}
