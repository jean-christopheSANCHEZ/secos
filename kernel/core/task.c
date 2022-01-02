#include <debug.h>
#include <segmem.h>
#include <info.h>
#include <idt_exam.h>
#include <intr.h>
#include <cr.h>
#include <pagemem.h>
#include <pagination.h>
#include <task.h>
#include <adresses.h>
#include <gdt.h>
#include <io.h>
#include <pic.h>

tache taches[2]; //2 taches maxi
tache *tache_en_cours = NULL;
tss_t *tss;
void print_taches(tache tache){
    printf("tache : num_user :%d, eip: %x, ebp: %x, espUser: %x, espNoyau: %x\n",tache.num_user, tache.eip, tache.ebp, tache.espUser, tache.espNoyau);
}

void ajouter_tache(uint32_t pileUser, uint32_t pileNoyau, void * user, uint32_t index, uint32_t num_user, pde32_t *pgd){
    taches[index].eip = (uint32_t)user;
    taches[index].num_user = num_user;
    taches[index].espNoyau = pileNoyau;
    taches[index].espUser = pileUser;
    taches[index].pgd = pgd;
    //eip stackKernel et user et @idt
    printf("translation :espNoyau %x espuser %x eip %x  pgd %x\n", taches[index].espNoyau, taches[index].espUser, taches[index].eip, taches[index].pgd);
    translation((uint32_t *)taches[index].espNoyau, (uint32_t *)taches[index].espNoyau, pgd);
    translation((uint32_t *)taches[index].espUser , (uint32_t *)taches[index].espUser, pgd);
    translation((uint32_t *)taches[index].eip, (uint32_t *)taches[index].eip, pgd);
    translation((uint32_t *)0x302010, (uint32_t *)0x302010, pgd);
    printf("fin de l'ajout de la tache %d\n", num_user);
}
void initTss(tss_t *extern_tss){
    tss = extern_tss;
}

void gestion_tache(){
    //printf("gestion_tache\n");
    print_taches(taches[0]);
    print_taches(taches[1]);
    outb(PIC_EOI, PIC1);
    if(tache_en_cours == NULL){
        printf("Pas de taches en cours \n");//pas de tache donc on fait la premier tache
        tache_en_cours = &taches[0];
        set_ds(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_es(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_fs(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_gs(gdt_seg_sel(gdt_indice_data_r3, 3));
        printf("Fin de la mag des segments data\n");
    }else{
        //on a une tache en cours : il faut donc la save puis charger l autre tache
        asm volatile("mov %%ebx, %0" : "=r"(tache_en_cours->ebx));
        asm volatile("mov %%esi, %0" : "=r"(tache_en_cours->esi));
        asm volatile("mov %%edi, %0" : "=r"(tache_en_cours->edi));
        asm volatile("pop %0" : "=r"(tache_en_cours->eip));
        asm volatile("pop %0" : "=r"(tache_en_cours->espUser));
        asm volatile("mov %%esp, %0" : "=r"(tache_en_cours->espNoyau));
        asm volatile("mov %%ebp, %0" : "=r"(tache_en_cours->ebp));
        printf("fin de la sauvegarde de la tache courante\n");
        if(tache_en_cours->num_user == 1){
            tache_en_cours = &taches[1];
            printf("changement de user 1 vers 2\n");
        }else if(tache_en_cours->num_user == 2){
            tache_en_cours = &taches[0];
            printf("changement de user 2 vers 1\n");
        }
        set_ds(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_es(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_fs(gdt_seg_sel(gdt_indice_data_r3, 3));
        set_gs(gdt_seg_sel(gdt_indice_data_r3, 3));
        printf("Fin de la mag des segments data\n");
        asm volatile("mov %0, %%esi" ::"r"(tache_en_cours->esi));
        asm volatile("mov %0, %%ebp" ::"r"(tache_en_cours->ebp));
        asm volatile("mov %0, %%esp" ::"r"(tache_en_cours->espNoyau));
        asm volatile("mov %0, %%edi" ::"r"(tache_en_cours->edi));
        asm volatile("mov %0, %%ebx" :: "r"(tache_en_cours->ebx));
        printf("Fin du chargement de la tache\n");
    }
    
    set_cr3(tache_en_cours->pgd);
    tss->s0.esp = tache_en_cours->espUser;
    printf("Maj de tss\n");
    printf("valeur pgd %x\n", tache_en_cours->pgd);
    asm volatile("push %0" ::"i"(gdt_seg_sel(gdt_indice_data_r3, 3)));
    asm volatile("push %0" ::"r"(tache_en_cours->espUser));
    save_flags(tache_en_cours->flags);
    tache_en_cours->flags = tache_en_cours->flags | EFLAGS_IF;
    asm volatile("push %0" :: "m"(tache_en_cours->flags));
    printf("pushed flags\n");
    asm volatile("push %0" ::"i"(gdt_seg_sel(gdt_indice_code_r3, 3)));
    asm volatile("push %%ebx" ::"b"((void *)tache_en_cours->eip));
    print_taches(*tache_en_cours);
    printf("fin gestion tache\n");
    asm volatile("iret");

}