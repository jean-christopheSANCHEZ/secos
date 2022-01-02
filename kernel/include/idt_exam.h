#ifndef IDT_EXAM_H
#define IDT_EXAM_H

#include <intr.h>

#define index_syscall_idt 0x80
#define index_irq0_idt 32

void syscall();
void syscall_handler(int_ctx_t contexte);
void new_handler(int ring, int index_idt, void *handler);

#endif