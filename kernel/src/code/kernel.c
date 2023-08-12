void main();
void _start(){
    main();
    while(1);
}

#include "screen.h"
#include "cpu/cpu.h"
#include "mem/mem.h"
#include "pci/pci.h"
#include "disk/disk.h"
#include "tools/checksum.h"

uint8_t MAXPHYADDR, MAXLINADDR;

uint64_t endofkernel;
uint64_t startofkernel;

void main(){

    //setup screen
    screenClear();
    screenPrint("---Kernel---/n/e");

    //setup pic (remap IRQs)
    uint8_t pic_0_base = 0x20;
    uint8_t pic_1_base = 0x28;
    screenPrint("Setup PICs... /e");
    pic_remap(pic_0_base, pic_1_base);
    screenPrint("OK/n/e");

    //init IDT
    screenPrint("Setup IDT... /e");
    idt_init();
    screenPrint("OK/n/e");

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

    screenPrint("Setup PIT IRQ... /e");
    syscounter_init();
    screenPrint("OK/n/e");

    //enable irqs  
    asm volatile("sti");

    //print CPU info
    char *vendorstr = cpuid_getVendor(); 
    uint32_t *cpu_features = cpuid_getFeatures();
    screenPrint("CPU vendor: /c/n/e",vendorstr);
    screenPrint("CPU feature ECX: /xD/n/e",cpu_features[0]);
    screenPrint("CPU feature EDX: /xD/n/e",cpu_features[1]);

    //get max of phy and lin addr range
    uint64_t reg;
    __asm__ __volatile__("movq $0x80000008, %%rax;"
                         "cpuid":
                         "=a"(reg):);
    MAXPHYADDR = (uint8_t) reg;
    MAXLINADDR = (uint8_t) (reg >> 8);
    screenPrint("PHY ADDR BITS: /xB/n/e",MAXPHYADDR);
    screenPrint("LIN ADDR BITS: /xB/n/e",MAXLINADDR);

    //get end of kernel
    extern uint64_t endptr;
    endofkernel = (uint64_t) &endptr;
    startofkernel = (uint64_t) &_start;

    //setup pageing
    screenPrint("Setup pageing system... /e");
    page_map_init(MAXPHYADDR, MAXLINADDR);
    screenPrint("OK/n/e");

    //setup page allocator
    screenPrint("Setup page allocator... /e");
    mem_palloc_init(&startofkernel, &endofkernel);
    screenPrint("OK/n/e");

    //setup memory allocator
    screenPrint("Setup memory allocator... /e");
    mem_init(&endofkernel, 0x10000, MAXLINADDR);
    screenPrint("OK/n/e");

    //print new end + start of kernel
    screenPrint("Kernel start: /xQ/n/e",startofkernel);
    screenPrint("Kernel end  : /xQ/n/e",endofkernel);
    

    print_memorymap();
    
    //print pci info
    pciCheckBus();
    pciPrintInfo();

    //search for disks
    int disk_error = disk_searchDisks();
    if(disk_error == 0){
        screenPrint("NO DISK FOUND IN SYSTEM!/n/e");
        while(1);
    }
    disk_init();

    
    
    //end of kernel
    screenPrint("DONE!/e");
    while(1);
}