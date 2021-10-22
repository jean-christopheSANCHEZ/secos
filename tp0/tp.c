/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

void tp() {
	debug("kernel mem [0x%x - 0x%x]\n", &__kernel_start__, &__kernel_end__);
	debug("MBI flags 0x%x\n", info->mbi->flags);
    
    mbi_t * mbi;
    multiboot_memory_map_t * debut;
    multiboot_memory_map_t * fin;

    mbi = info->mbi;
    debut = (multiboot_memory_map_t *) mbi->mmap_addr;
    fin = (multiboot_memory_map_t *) (mbi->mmap_addr + mbi->mmap_length);
    debug("INIT     debut = 0x%llx      fin = 0x%llx", debut->addr, debut->addr + debut->len);
    while(debut < fin){
        debug("debut = 0x%llx      fin = 0x%llx     (%d)\n", debut->addr, debut->addr + debut->len, debut->type);
        debut ++;    
    }

    int * ptr;
    ptr =(int *) 0x9fc00;
    printf("ptr = %p\n",* ptr);

    
    int * ptr1;
    ptr1 =(int *) 0xf0000;
    printf("ptr1 = %p\n",* ptr1);
    
    
    int * ptr2;
    ptr2 =(int *) 0x10000;
    printf("ptr2 = %p\n",* ptr2);


    int * ptr3;
    ptr3 =(int *) 0x7fe0000;
    printf("ptr3 = %p\n",* ptr3);
}
