#ifndef ADRESSES_H
#define ADRESSES_H

#define pgdKERNEL (pde32_t*)0x500000
#define ptbKERNEL (pte32_t*)0x510000
#define pgdTACHE1 (pde32_t*)0x520000
#define ptbTACHE1 (pte32_t*)0x900000
#define pgdTACHE2 (pde32_t*)0x540000
#define ptbTACHE2 (pte32_t*)0xc00000
#define MEM_VIRT_USER1 0x600000
#define MEM_VIRT_USER2 0x650000
#define MEM_PHYSIQUE_USER12 0x700000
#define PILE_USER1_0 0x800fff
#define PILE_USER1_3 0x810fff
#define PILE_USER2_0 0x840fff
#define PILE_USER2_3 0x830fff

#endif