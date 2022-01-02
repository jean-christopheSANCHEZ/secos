#ifndef TASK_H
#define TASK_H
#include <pagemem.h>

typedef struct tache{
    pde32_t  *pgd;
    uint32_t num_user;
    uint32_t eip;
    uint32_t ebp;
    uint32_t  ebx;
    uint32_t  esi;
    uint32_t  edi;
    uint32_t  espNoyau;
    uint32_t  espUser;
    uint32_t flags;

}tache;


void gestion_tache();
void ajouter_tache(uint32_t pileUser, uint32_t pileNoyau, void * user, uint32_t index, uint32_t num_user, pde32_t *pgd);

void initTss(tss_t *extern_tss);
#endif