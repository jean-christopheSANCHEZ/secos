#ifndef PAGINATION_H
#define PAGINATION_H

#include <pagemem.h>

void print_cr3();
void active_pagination();
void init_pagination_kernel();
void init_pagination_user();
void translation(uint32_t *addr_virtuelle, uint32_t *addr_physique, pde32_t *pgd);
void print_pages(pde32_t *pgd);
void print_pte(pte32_t *pte, uint32_t index);
void prepare_pagination();

#endif