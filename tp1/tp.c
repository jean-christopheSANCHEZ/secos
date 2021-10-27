/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

extern info_t *info;


/*init my own gdt*/
gdt_reg_t new_gdtr;
seg_desc_t gdtSegments[4];

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

void fill_gdt(seg_desc_t * gdtSegments, uint32_t limit, uint32_t base, uint64_t type, uint64_t s, uint64_t dpl){
    gdtSegments->raw = 0ULL;
    
    //set the limit bits of the seg
    gdtSegments->limit_1 = limit & 0xFFFF; //on prend uniquement les 16 bits
    gdtSegments->limit_2 = (limit >> 16) & 0xF; //on decale de 16bits puis on prend 4 bits

    //set the base bits of the seg
    gdtSegments->base_1 = base & 0xFFFF;
    gdtSegments->base_2 = (base >> 16) & 0xff;
    gdtSegments->base_3 = (base >> 24) & 0xff;

    //set the type
    gdtSegments->type = type;

    // set s : descriptor type
    gdtSegments->s = s;

    //set dpl : descriptor privilege level
    gdtSegments->dpl = dpl;

    //set p : segment present flag : 1
    gdtSegments->p = 1;

    //set l : longmode : 0 because 32bits
    gdtSegments->l = 0;

    //set d : default lenght : 1 because we are 32bits
    gdtSegments->d = 1;

    //set granularity
    gdtSegments->g = 1;

    //set avl : 0
    gdtSegments->avl = 0;

    gdtSegments->raw = 0ULL;
}   


void init_gdt(){
        //init the first segment 0 indice : NULL value
    gdtSegments[0].raw = 0ULL;

    // init the second segment : 1 indice for code
    fill_gdt(&gdtSegments[1],
        /*limit addr max on 32 bits : 0xFFFFFF*/0xFFFFFF,
         /*base addr : 0*/0 ,
         /*type : rx*/ SEG_DESC_CODE_XR,
         /*s because type code : 1 */1,
         /*dpl : ring0 : 0*/ 0
    );

    // init the third segment : 2 indice for data
    fill_gdt(&gdtSegments[2],
        /*limit addr max on 32 bits : 0xFFFFFF*/0xFFFFFF,
         /*base addr : 0*/0 ,
         /*type : rx*/ SEG_DESC_DATA_RW,
         /*s because type data : 1 */1,
         /*dpl : ring0 : 0*/ 0
    );

    //init the last segment at NULL
    gdtSegments[3].raw = 0ULL;


    //init gdt : 
    //gdtr limit :
    new_gdtr.limit = sizeof(gdtSegments) - 1;
    //init the segments
    new_gdtr.desc = gdtSegments;

    //load my new gdtr
    set_gdtr(new_gdtr);

    //on charge la nouvelle gdtr dans les registres de segments
    set_cs(gdt_krn_seg_sel(1));

    set_ds(gdt_krn_seg_sel(2));
    set_es(gdt_krn_seg_sel(2));
    set_ss(gdt_krn_seg_sel(2));
    set_fs(gdt_krn_seg_sel(2));
    set_gs(gdt_krn_seg_sel(2));
}

void f(){
    char  src[64];
    char *dst = 0;
    memset(src, 0xff, 64);
    
}

void tp(){
    
    //print this gdt
    //print_gdtr();   

    // init my own gdt
    init_gdt();
    print_gdtr();
    
}
