#ifndef _ARCH_I386_GDT_H
#define _ARCH_I386_GDT_H

#include <stdint.h>

struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t  base_middle;
  uint8_t  access;
  uint8_t  granularity;
  uint8_t  base_high;
} __attribute__ ((packed));

struct gdt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__ ((packed));

void init_gdt ();
void set_kernel_stack (uint32_t stack);

extern void gdt_flush ();

#endif
