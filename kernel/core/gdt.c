#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <gdt.h>

//entrées de la gdt
seg_desc_t gdtSegments[6];




#define tss_dsc(_dSc_,_tSs_)                                            \
   ({                                                                   \
      raw32_t addr    = {.raw = _tSs_};                                 \
      (_dSc_)->raw    = sizeof(tss_t);                                  \
      (_dSc_)->base_1 = addr.wlow;                                      \
      (_dSc_)->base_2 = addr._whigh.blow;                               \
      (_dSc_)->base_3 = addr._whigh.bhigh;                              \
      (_dSc_)->type   = SEG_DESC_SYS_TSS_AVL_32;                        \
      (_dSc_)->p      = 1;                                              \
   })

//print la gdt
void print_gdtr(){
    gdt_reg_t my_gdtr;
    get_gdtr(my_gdtr);
    int nb_segment = 0;
    nb_segment = (my_gdtr.limit +1 ) / sizeof(seg_desc_t);
    printf("\n\n                     =========GDTR=========n\n");
    printf("        limit                    : %x\n",my_gdtr.limit);
    printf("        offset                   : %x\n", my_gdtr.addr);
    printf("                             ==SEGs DESC==    \n\n");
    printf("        nombre de segemnts       : %d\n", nb_segment);
    for(int i =0; i<nb_segment; i++){
        printf("SEG n'%d\n", i+1);
        if(my_gdtr.desc[i].p == 1){
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


//rempli la gdt
void fill_gdt(uint32_t indice_seg, uint32_t limit, uint32_t base, uint32_t type, uint32_t dpl){
    gdtSegments[indice_seg].raw = 0ULL;
    
    //set the limit bits of the seg
    gdtSegments[indice_seg].limit_1 = limit & 0xFFFF; //on prend uniquement les 16 bits
    gdtSegments[indice_seg].limit_2 = (limit >> 16) & 0x0F; //on decale de 16bits puis on prend 4 bits

    //set the base bits of the seg
    gdtSegments[indice_seg].base_1 = base & 0xFFFF;
    gdtSegments[indice_seg].base_2 = (base >> 16) & 0xff;
    gdtSegments[indice_seg].base_3 = (base >> 24) & 0xff;

    //set the type
    gdtSegments[indice_seg].type = type;
    // set s : descriptor type
    gdtSegments[indice_seg].s = 1;
    //set dpl : descriptor privilege level
    gdtSegments[indice_seg].dpl = dpl;
    //set p : segment present flag : 1
    gdtSegments[indice_seg].p = 1;
    //set l : longmode : 0 because 32bits
    gdtSegments[indice_seg].l = 0;
    //set d : default lenght : 1 because we are 32bits
    gdtSegments[indice_seg].d = 1;
    //set granularity
    gdtSegments[indice_seg].g = 1;
    //set avl : 0
    gdtSegments[indice_seg].avl = 0;
}


//init la gdt
void init_gdt(){
    gdt_reg_t gdtr;
    //gdtr limit
    gdtr.limit = sizeof(gdtSegments) - 1;
    //init the segments
    gdtr.desc = gdtSegments;
    //init the first segment 0 indice : NULL value
    fill_gdt(0,0,0,0,0);
    //load my new gdtr
    set_gdtr(gdtr);
}

//init tss
void init_tss(tss_t *tss){
    tss_dsc(&gdtSegments[5], (offset_t)tss);
    tss->s0.ss = gdt_krn_seg_sel(gdt_indice_data_r0);
    tss->s0.esp = get_ebp();
    //load segments selectors
    set_tr(gdt_krn_seg_sel(gdt_indice_tss));
    set_cs(gdt_krn_seg_sel(gdt_indice_code_r0));
    set_ds(gdt_krn_seg_sel(gdt_indice_data_r0));
    set_es(gdt_krn_seg_sel(gdt_indice_data_r0));
    set_ss(gdt_krn_seg_sel(gdt_indice_data_r0));
    set_fs(gdt_krn_seg_sel(gdt_indice_data_r0));
    set_gs(gdt_krn_seg_sel(gdt_indice_data_r0));

}
