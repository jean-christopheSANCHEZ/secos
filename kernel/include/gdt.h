#ifndef GDT_H
#define GDT_H

#include <debug.h>
#include <info.h>
#include <segmem.h>


//indice des segments de la gdt
#define gdt_indice_code_r0 1 //0 est nul 1 c'est le premier dispo
#define gdt_indice_data_r0 2
#define gdt_indice_code_r3 3
#define gdt_indice_data_r3 4
#define gdt_indice_tss 5


void print_gdtr();
void fill_gdt(uint32_t indice_seg, uint32_t limit, uint32_t base, uint32_t type, uint32_t dpl);
void init_gdt();
void init_tss(tss_t *tss);
#endif