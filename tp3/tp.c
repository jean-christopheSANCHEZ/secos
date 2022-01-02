/*// GPLv2 (c) Airbus
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

extern info_t *info;

//init my own gdt
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
}


void init_gdt(){
        //init the first segment 0 indice : NULL value
    gdtSegments[0].raw = 0ULL;

    // init the second segment : 1 indice for code
    fill_gdt(&gdtSegments[1],
         0xFFFFFF,
         base addr : 0
         0 ,
         //type : rx
          SEG_DESC_DATA_RW,
         //s because type data : 1 
         1,
         //dpl : ring0 : 0
         0
    );

    // init the third segment : 2 indice for data
    fill_gdt(&gdtSegments[2],
        //limit addr max on 32 bits : 0xFFFFFF
        0xFFFFFF,
         base addr : 0
         0 ,
         //type : rx
          SEG_DESC_DATA_RW,
         //s because type data : 1 
         1,
         //dpl : ring0 : 0
         0
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


void userland()
{
   asm volatile ("mov %eax, %cr0");
}


void tp()
{

}
*/
/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmem.h>
#include <info.h>

extern info_t *info;

#define c0_idx  1
#define d0_idx  2
#define c3_idx  3
#define d3_idx  4
#define ts_idx  5

#define c0_sel  gdt_krn_seg_sel(c0_idx)
#define d0_sel  gdt_krn_seg_sel(d0_idx)
#define c3_sel  gdt_usr_seg_sel(c3_idx)
#define d3_sel  gdt_usr_seg_sel(d3_idx)
#define ts_sel  gdt_krn_seg_sel(ts_idx)

seg_desc_t GDT[6];
tss_t      TSS;

#define gdt_flat_dsc(_dSc_,_pVl_,_tYp_)                                 \
   ({                                                                   \
      (_dSc_)->raw     = 0;                                             \
      (_dSc_)->limit_1 = 0xffff;                                        \
      (_dSc_)->limit_2 = 0xf;                                           \
      (_dSc_)->type    = _tYp_;                                         \
      (_dSc_)->dpl     = _pVl_;                                         \
      (_dSc_)->d       = 1;                                             \
      (_dSc_)->g       = 1;                                             \
      (_dSc_)->s       = 1;                                             \
      (_dSc_)->p       = 1;                                             \
   })

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

#define c0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_CODE_XR)
#define d0_dsc(_d) gdt_flat_dsc(_d,0,SEG_DESC_DATA_RW)
#define c3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_CODE_XR)
#define d3_dsc(_d) gdt_flat_dsc(_d,3,SEG_DESC_DATA_RW)

void init_gdt()
{
   gdt_reg_t gdtr;

   GDT[0].raw = 0ULL;

   c0_dsc( &GDT[c0_idx] );
   d0_dsc( &GDT[d0_idx] );
   c3_dsc( &GDT[c3_idx] );
   d3_dsc( &GDT[d3_idx] );

   gdtr.desc  = GDT;
   gdtr.limit = sizeof(GDT) - 1;
   set_gdtr(gdtr);

   set_cs(c0_sel);

   set_ss(d0_sel);
   set_ds(d0_sel);
   set_es(d0_sel);
   set_fs(d0_sel);
   set_gs(d0_sel);
}

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

void userland()
{
   asm volatile ("mov %eax, %cr0");
   printf("ENFIN userland !\n");
   while(1);
}

void test_user()
{
    printf("debut\n");
   set_ds(d3_sel);
   set_es(d3_sel);
   set_fs(d3_sel);
   set_gs(d3_sel);
   set_ss(d3_sel);
    printf("test_user après avoir set les registres\n");
   fptr32_t fptr = {.segment = c3_sel, .offset = (uint32_t)userland}; 
   farjump(fptr); 
    debug("test_user apres avoir farjump\n");
   asm volatile ( 
       "push %0    \n" // ss 
       "push %%ebp \n" // esp 
       "pushf      \n" // eflags 
       "push %1    \n" // cs 
       "push %2    \n" // eip 
       "iret" 
       :: 
        "i"(d3_sel),   //ss 
        "i"(c3_sel),   //cs 
        "r"(&userland) //eip 
       ); 

   TSS.s0.esp = get_ebp();
   TSS.s0.ss  = d0_sel;
   tss_dsc(&GDT[ts_idx], (offset_t)&TSS);
   set_tr(ts_sel);

   asm volatile (
      "push %0    \n" // ss
      "push %%ebp \n" // esp
      "pushf      \n" // eflags
      "push %1    \n" // cs
      "push %2    \n" // eip
      "iret"
      ::
       "i"(d3_sel),
       "i"(c3_sel),
       "r"(&userland)
      );
}


void tp()
{
   print_gdtr();
   init_gdt();
   print_gdtr();
   test_user();

}
