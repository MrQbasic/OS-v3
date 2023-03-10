#include "cpu/idt.h"
#include "screen.h"

struct IDT64 IDT[256];

struct IDTR_64 IDTR;

void idt_set(uint64_t offset, uint16_t segSel, uint8_t idte, uint8_t flags){
   //IDT[idte].offset_1 = (uint16_t) offset;
   //IDT[idte].offset_2 = (uint32_t) (offset >> 16);
   //IDT[idte].offset_3 = (uint32_t) (offset >> 32);
   IDT[idte].selector = segSel;
   IDT[idte].type_attributes = flags;
   IDT[idte].zero = 0;
   IDT[idte].ist = 0;
   
}

char msgint[] = "INT!/n/e";

void load_idt (struct IDTR_64 *idtrp)
{
   __asm__ __volatile__("lidtq (%0)":: "a"(idtrp));
}

void  idt_init(){               
   //get ptr to external default isr
   extern void idt_isr_default();
   //setupt idt
   for(int i=0; i<100; i++){
      idt_set((uint64_t) idt_isr_default, IDT_DEFAULT_SEGSEL, i, IDT_DEFAULT_FLAG);
   }
   //setupt idtr
   IDTR.size = sizeof(IDT)-1;
   IDTR.offset = (uintptr_t) &IDT[0];
   load_idt(&IDTR);
}