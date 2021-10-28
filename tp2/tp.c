/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <intr.h>
#include <segmem.h>

extern info_t *info;


void bp_handler(){
    debug("_bp_handler\n");
    //on doit push tout le context et donc tout les registres
    //asm volatile("pusha");

    //on restaure le context
    //asm volatile("popa");

    //iret à la fin de l interruption
    asm volatile("leave; iret");
}

void bp_trigger(){
    asm volatile("int3");
    debug("bp_trigerred\n");
}

void tp()
{
    /*genere interruption en faisant un divide by zero*/
    /*int a = 0;
    int b = 10;
    int c = 0;
    while(1){
        c = b/a;
        printf("c = %d\n", c);
    }*/

    idt_reg_t load_addr;
    get_idtr(load_addr);
    printf("addr de idtr : %x\n", load_addr.addr);

    load_addr.desc[BP_EXCP].offset_1 = (uint16_t) ((uint32_t)bp_handler);// & 0xFFFF);
    load_addr.desc[BP_EXCP].offset_2 = (uint16_t) ((uint32_t)bp_handler >>16);// & 0xFFFF);
    bp_trigger();
    printf("FIN MAIN\n");
}
