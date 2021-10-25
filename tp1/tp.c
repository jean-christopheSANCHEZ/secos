/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
extern info_t *info;


void print_gdtr(gdt_reg_t my_gdtr){
    int nb_segment = 0;
    nb_segment = (my_gdtr.limit +1 ) / sizeof(seg_desc_t);
    printf("\n\n                     =========GDTR=========n\n");
    printf("        limit                    : %x\n",my_gdtr.limit);
    printf("        offset                   : %x\n", my_gdtr.addr);
    printf("                             ==SEGs DESC==    \n\n");
    printf("        nombre de segemnts       : %d\n", nb_segment);
    for(int i =0; i<nb_segment; i++){
        if(my_gdtr.desc[i].p == 1){
            printf("SEG n'%d\n", i+1);
            printf("        segment limit            : %lx\n", my_gdtr.desc[i].limit_1 | (my_gdtr.desc[i].limit_2 << 16));

            printf("        adresse de base          : %lx\n", my_gdtr.desc[i].base_1 |( my_gdtr.desc[i].base_2 << 16) | (my_gdtr.desc[i].base_3 << 24));
            printf("        type du segment          : %lx\n", my_gdtr.desc[i].type);
            printf("        type de descripteur      : %lx\n", my_gdtr.desc[i].s);
            printf("        privilege du descripteur : %lx\n", my_gdtr.desc[i].dpl);
            printf("        granularite              : %lx\n", my_gdtr.desc[i].g);
        }else{
            printf("        Segment Nul\n");
        }
    }
}

void init_gdt(gdt_reg_t * gdt){
    
}

void tp(){
    gdt_reg_t my_gdtr;
    get_gdtr(my_gdtr);

    print_gdtr(my_gdtr);

    init_gdt();
    
}
