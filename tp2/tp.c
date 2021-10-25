/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>

extern info_t *info;

void tp()
{
    /*int a = 0;
    int b = 10;
    int c = 0;
    while(1){
        c = b/a;
        printf("c = %d\n", c);
    }*/

    idt_reg_t load_addr;
    printf("addr de idtr : %x\b", get_idtr(load_addr.desc));
}
