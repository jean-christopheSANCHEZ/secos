#include <debug.h>
#include <segmem.h>
#include <types.h>
#include <info.h>
#include <idt_exam.h>
#include <intr.h>
#include <pic.h>
#include <task.h>
#include <adresses.h>
//#include <intr.h>

void syscall(){
    asm volatile("leave");
    asm volatile ("pusha");
    asm volatile ("mov %esp, %eax");
    printf("syscall\n");
    asm volatile("call print_syscall");
    //outb(PIC_EOI, PIC1);
    asm volatile ("popa ; iret");
}

void print_syscall(int_ctx_t contexte){
    printf("print_syscall : %p \n", contexte.gpr.eax.raw);
}

void new_handler(int ring, int index_idt, void *handler){
    idt_reg_t idtr;
    get_idtr(idtr);
    printf("addr IDT : %x\n", idtr.addr);
    int_desc_t *syscall_dsc = &idtr.desc[index_idt];
    syscall_dsc->dpl = ring;
    syscall_dsc->type = SEG_DESC_SYS_TRAP_GATE_32;
    syscall_dsc->p = 1;
    int_desc(&idtr.desc[index_idt], gdt_krn_seg_sel(1), (offset_t)handler);
}