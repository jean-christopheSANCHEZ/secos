#include <debug.h>
#include <segmem.h>
#include <info.h>
#include <idt_exam.h>
#include <intr.h>
#include <cr.h>
#include <pagemem.h>
#include <pagination.h>
#include <adresses.h>

extern info_t *info;
pde32_t *pgd_kernel = (pde32_t *)pgdKERNEL;
pde32_t *pgd_tache_1 = (pde32_t *)pgdTACHE1;
pde32_t *pgd_tache_2 =(pde32_t *) pgdTACHE2;

void print_cr3(){
    cr3_reg_t cr3 = {.raw = get_cr3()};
    printf("cr3 = %p\n", cr3); //addresse de la PGD
}

void active_pagination(){
   uint32_t cr0 = get_cr0();
   set_cr0(cr0|CR0_PG);
   printf("pagination activated \n");
}

void init_pagination_kernel(){
    //kernel part
    printf("init kernel pagination\n");
    
    pte32_t *ptb_kernel = ptbKERNEL;

    memset((void*)pgd_kernel, 0, PAGE_SIZE);
    for(int i =0;i<1024;i++){
        pg_set_entry(&ptb_kernel[i], PG_KRN|PG_RW, i);
    }
    pg_set_entry(&pgd_kernel[0], PG_KRN|PG_RW, page_nr(ptb_kernel));
    printf("end init kernel pagination\n");
}

void page_user(pde32_t *pgd, pte32_t *pte){
    memset((void*)pgd, 0, PAGE_SIZE);
    for(int i =0;i<1024;i++){
        pg_set_entry(&pte[i], PG_USR|PG_RW, i);
    }
    pg_set_entry(pgd, PG_USR|PG_RW, page_nr(pte));
}

void init_pagination_user(){
    //tache 1 part
    pte32_t *ptb_tache_1 = ptbTACHE1;
    page_user(pgd_tache_1, ptb_tache_1);
    page_user(pgd_tache_1 + 1, ptb_tache_1 + 0x1000); //1ere rangée
    page_user(pgd_tache_1 + 2, ptb_tache_1 + 0x2000); //2eme rangée
    //page_user(pgd_tache_1 + 3, ptb_tache_1 + 0x3000); //3eme rangée pas nécessaire

    //tache 2 part
    pte32_t *ptb_tache_2 = ptbTACHE2;
    page_user(pgd_tache_2, ptb_tache_2);
    page_user(pgd_tache_2 + 1, ptb_tache_2 + 0x1000); //1ere rangée
    page_user(pgd_tache_2 + 2, ptb_tache_2 + 0x2000); //2eme rangée
    //page_user(pgd_tache_2 + 3, ptb_tache_2 + 0x3000); //3eme rangée pas nécessaire
}

void print_pages(pde32_t *pgd){
    printf("Printf PGD : %x\n",pgd);
    for(uint32_t i=0; i< PDE32_PER_PD; i++){
        if(pgd[i].p == 1){
            printf("Entrée num %d, ADDR ptb: %x, DPL: %d \n", i, pgd[i].addr<<12, pgd[i].lvl);
            print_pte((pte32_t *)(pgd[i].addr<<12), i<<10);
        }
    }
}

void print_pte(pte32_t *pte, uint32_t index){
    for(uint32_t i=0; i< PTE32_PER_PT; i++){
        if(pte[i].p == 1){
            printf("    PTB_ID : %d, ADDR phy: %x , ADDR virt: %x, DPL: %d\n", i, pte[i].addr<<12, (index | i) <<12, pte[i].lvl);
        }
    }
}

void translation(uint32_t *addr_virtuelle, uint32_t *addr_physique, pde32_t *pgd){
    uint32_t pgd_idx = pd32_idx(addr_virtuelle);
    uint32_t ptb_idx = pt32_idx(addr_virtuelle);

    pte32_t  *ptb_tmp    = (pte32_t*)page_addr(pgd[pgd_idx].addr);
    pg_set_entry(&ptb_tmp[ptb_idx], PG_USR|PG_RW, page_nr(addr_physique));
    //pg_set_entry(&pgd[pgd_idx], PG_USR|PG_RW, page_nr(ptb_tmp));

    //printf("PGD[0] = %p | addr_virtuelle = %p\n", pgd[0].raw, addr_virtuelle);
}


void prepare_pagination(){
    init_pagination_kernel();
    init_pagination_user();
    //translation pour mem partaguée
    translation((uint32_t *)MEM_VIRT_USER1, (uint32_t *)MEM_PHYSIQUE_USER12, pgd_tache_1);
    translation((uint32_t *)MEM_VIRT_USER2, (uint32_t *)MEM_PHYSIQUE_USER12, pgd_tache_2);
    printf("MEM_VIRT_USER1 %x MEM_PHYSIQUE_USER12 %x MEM_VIRT_USER2 %x\n ", (uint32_t *)MEM_VIRT_USER1, (uint32_t *)MEM_PHYSIQUE_USER12, (uint32_t *)MEM_VIRT_USER2);
    /*print_pages(pgd_tache_1);
    print_pages(pgd_tache_2);
    while(1);*/    
    set_cr3(pgd_kernel);
}