#include "def.h"

struct IDT64 {
   uint16_t offset_1;
   uint16_t selector;
   uint8_t  ist;
   uint8_t  type_attributes;
   uint16_t offset_2;
   uint32_t offset_3;
   uint32_t zero;
};

struct IDTR_64 {
   uint16_t size;
   uintptr_t offset;
}__attribute__((packed));

#define IDT_DEFAULT_SEGSEL 0x08 
#define IDT_DEFAULT_FLAG   0x8E

void idt_set(uint64_t offset, uint16_t segSel, uint8_t idte, uint8_t flags);

void print_idte(int int_index);

void idt_init();

