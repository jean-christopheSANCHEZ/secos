/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <cr.h>
#include <asm.h>
#include <pagemem.h>
#include <gdt.h>
#include <idt_exam.h>
#include <pagination.h>
#include <task.h>
#include <adresses.h>

extern info_t *info;
tss_t tss;
extern pde32_t *pgd_tache_1;
extern pde32_t *pgd_tache_2;

void sys_counter(uint32_t *counter){
    asm volatile("mov %0, %%eax" :: "r"(counter));
    //asm volatile("int $0x80");
    printf("SYS_CONTER \n");
}

void incr_compteur(uint32_t *cpt){
    *cpt = *cpt + 1;
    printf("incr_compteur compteur :%d\n", *cpt);
}

__attribute__ ((section(".user1")))void user1(){
    //printf("USER1\n");
    uint32_t * compteur = (uint32_t *)MEM_VIRT_USER1;
    *compteur = 0;
    while(1){
        incr_compteur(compteur);
    }
}

__attribute__ ((section(".user2")))void user2(){
    //printf("USER2\n");
    uint32_t * compteur = (uint32_t *)MEM_VIRT_USER2;
    *compteur = 0;
    while(1){
        sys_counter(compteur);
    }
}

void prepare_gdt(){
    init_gdt();
    fill_gdt(gdt_indice_code_r0, 0xFFFFF, 0, SEG_DESC_CODE_XR, 0);
    fill_gdt(gdt_indice_data_r0, 0xFFFFF, 0, SEG_DESC_DATA_RW, 0);
    fill_gdt(gdt_indice_code_r3, 0xFFFFF, 0, SEG_DESC_CODE_XR, 3);
    fill_gdt(gdt_indice_data_r3, 0xFFFFF, 0, SEG_DESC_DATA_RW, 3);
}

void tp()
{
    //init gdt
    prepare_gdt();
    init_tss(&tss);
    //init idt
    intr_init();
    idt_reg_t idtr;
    get_idtr(idtr);
    printf("addr IDT : %p\n", &idtr.addr);
    new_handler(3, index_syscall_idt, syscall);
    new_handler(0, index_irq0_idt, gestion_tache);
    //pagination
    prepare_pagination();
    // on doit initialiser tss puis les taches ici
    initTss(&tss);
    ajouter_tache(PILE_USER1_0, PILE_USER1_3, user1, 0, 1, pgd_tache_1);
    ajouter_tache(PILE_USER2_0, PILE_USER2_3, user2, 1, 2, pgd_tache_2);
    active_pagination();
    force_interrupts_on();
    while(1){
        
    }    
}
