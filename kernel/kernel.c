void main();
void _start(){
    main();
    while(1);
}

#include "screen.h"
#include "cpu/cpu.h"
#include "mem/mem.h"
#include "pci/pci.h"

char kernelStartMsg[] = "---Kernel---/n/e";
char kernelPicMsg[]   = "Setup PICs/e";
char kernelIdtMsg[]   = "Setup ITD/e";
char kernelCntMsg[]   = "Setup PIT IRQ/e";
char kernelPageMsg[]  = "Setup pageing system/e";
char kernelPMemMsg[]  = "Setup page allocator/e";
char kernelMemMsg[]   = "Setup memory allocator/e";
char kernelOkMsg[]    = " OK/n/e";

uint8_t MAXPHYADDR, MAXLINADDR;

uint64_t endofkernel;
uint64_t startofkernel;

void main(){
    //setup screen
    screenClear();
    screenPrintChars(kernelStartMsg);

    //setup pic (remap IRQs)
    uint8_t pic_0_base = 0x20;
    uint8_t pic_1_base = 0x28;
    screenPrintChars(kernelPicMsg);
    pic_remap(pic_0_base, pic_1_base);
    screenPrintChars(kernelOkMsg);

    //init IDT
    screenPrintChars(kernelIdtMsg);
    idt_init();
    screenPrintChars(kernelOkMsg);

    //setup exceptions
    idt_set((uint64_t)  divByZeroISR, 0x08, 0x00, EXC_IDT_FLAG);
    idt_set((uint64_t)      debugISR, 0x08, 0x01, EXC_IDT_FLAG);
    idt_set((uint64_t)        nmiISR, 0x08, 0x02, EXC_IDT_FLAG);
    idt_set((uint64_t) breakpointISR, 0x08, 0x03, EXC_IDT_FLAG);
    idt_set((uint64_t)   overflowISR, 0x08, 0x04, EXC_IDT_FLAG);
    idt_set((uint64_t)      boundISR, 0x08, 0x05, EXC_IDT_FLAG);
    idt_set((uint64_t)     opcodeISR, 0x08, 0x06, EXC_IDT_FLAG);
    idt_set((uint64_t)     deviceISR, 0x08, 0x07, EXC_IDT_FLAG);
    idt_set((uint64_t)   doubleF_ISR, 0x08, 0x08, EXC_IDT_FLAG);
    idt_set((uint64_t)       TSS_ISR, 0x08, 0x0A, EXC_IDT_FLAG);
    idt_set((uint64_t)    segPresISR, 0x08, 0x0B, EXC_IDT_FLAG);
    idt_set((uint64_t)   segStackISR, 0x08, 0x0C, EXC_IDT_FLAG);
    idt_set((uint64_t)        gpfISR, 0x08, 0x0D, EXC_IDT_FLAG);
    idt_set((uint64_t)         pfISR, 0x08, 0x0E, EXC_IDT_FLAG);
    idt_set((uint64_t)        x87ISR, 0x08, 0x10, EXC_IDT_FLAG);
    idt_set((uint64_t)  alignmentISR, 0x08, 0x11, EXC_IDT_FLAG);
    idt_set((uint64_t)    machineISR, 0x08, 0x12, EXC_IDT_FLAG);
    idt_set((uint64_t)       simdISR, 0x08, 0x13, EXC_IDT_FLAG);
    idt_set((uint64_t)       virtISR, 0x08, 0x14, EXC_IDT_FLAG);
    idt_set((uint64_t)        crtISR, 0x08, 0x15, EXC_IDT_FLAG);
    idt_set((uint64_t)      hyperISR, 0x08, 0x1C, EXC_IDT_FLAG);
    idt_set((uint64_t)        vmmISR, 0x08, 0x1D, EXC_IDT_FLAG);
    idt_set((uint64_t)        secISR, 0x08, 0x1E, EXC_IDT_FLAG);

    //setup system counter
    screenPrintChars(kernelCntMsg);
    syscounter_init();
    screenPrintChars(kernelOkMsg);

    //enable irqs    
    //__asm__("sti");

    //print CPU info
    char *vendorstr = cpuid_getVendor(); 
    uint32_t *cpu_features = cpuid_getFeatures();
    char vendor[] = "CPU vendor string: /e";
    char feat1[]  = "CPU feature ECX: /e";
    char feat2[]  = "CPU feature EDX: /e";
    screenPrintChars(vendor);
    screenPrintChars(vendorstr); screenNl();
    screenPrintChars(feat1);screenPrintX32(cpu_features[0]); screenNl();
    screenPrintChars(feat2);screenPrintX32(cpu_features[1]); screenNl();

    //get max of phy and lin addr range
    uint64_t reg;
    __asm__ __volatile__("movq $0x80000008, %%rax;"
                         "cpuid":
                         "=a"(reg):);
    MAXPHYADDR = (uint8_t) reg;
    MAXLINADDR = (uint8_t) (reg >> 8);
    char PHYSTR[] = "PHY ADDR BITS: /e";
    char LINSTR[] = "LIN ADDR BITS: /e";
    screenPrintChars(PHYSTR);screenPrintX8(MAXPHYADDR);screenNl();
    screenPrintChars(LINSTR);screenPrintX8(MAXLINADDR);screenNl();

    //get end of kernel
    extern uint64_t endptr;
    endofkernel = (uint64_t) &endptr;
    startofkernel = (uint64_t) &_start;

    //setup page allocator
    screenPrintChars(kernelPMemMsg);
    mem_palloc_init(&endofkernel);
    screenPrintChars(kernelOkMsg);

    //setup pageing
    screenPrintChars(kernelPageMsg);
    page_map_init(MAXPHYADDR, MAXLINADDR);
    screenPrintChars(kernelOkMsg);

    //setup memory allocator
    screenPrintChars(kernelMemMsg);
    mem_init(&endofkernel, 0x10000, MAXLINADDR);
    screenPrintChars(kernelOkMsg);

    //print new end + start of kernel
    char startMsg[] = "Kernel start: /e";
    screenPrintChars(startMsg);screenPrintX64(startofkernel);screenNl();
    char endMsg[]   = "Kernel end  : /e";
    screenPrintChars(endMsg);screenPrintX64(endofkernel);screenNl();
    
    pciCheckBus();
    pciPrintInfo();
    
    //end of kernel
    screenPrintChars(kernelOkMsg);
    while(1);
}