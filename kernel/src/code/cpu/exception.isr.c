#include "cpu/exception.h"
#include "screen.h"

char divByZero[]  = "EXCEPTION: DIVIDE BY ZERO/e";
char debug[]      = "EXCEPTION: DEBUG/e";
char nmi[]        = "EXCEPTION: NMI/e";
char breakpoint[] = "EXCEPTION: BREAKPOINT/e";
char overflow[]   = "EXCEPTION: OVERFLOW/e";
char bound[]      = "EXCEPTION: BOUND RANGE EXCEEDED/e";
char opcode[]     = "EXCEPTION: INVALID OPCODE/e";
char device[]     = "EXCEPTION: DEVICE NOT AVAILABLE/e";
char doubleF[]    = "EXCEPTION: DOUBLE FAULT/e";
char TSS[]        = "EXCEPTION: INVALID TSS/e";
char segPres[]    = "EXCEPTION: SEGMENT NOT PRESENT/e"; 
char segStack[]   = "EXCEPTION: STACK SEGMENT FAULT/e";
char gpf[]        = "EXCEPTION: GENERAL PROTECTION FAULT/e";
char pf[]         = "EXCEPTION: PAGE FAULT/e";
char x87[]        = "EXCEPTION: X86 FLOATING POINT EXCEPTION/e";
char alignment[]  = "EXCEPTION: ALIGNMENT CHECK/e";
char machine[]    = "EXCEPTION: MACHONE CHECK/e";
char simd[]       = "EXCEPTION: SIMD FLOATING POINT EXCEPTION/e";
char virt[]       = "EXCEPTION: VIRTUALIZATION EXCEPTION/e";
char crt[]        = "EXCEPTION: CONTROL PROTECTION EXCEPTION/e";
char hyper[]      = "EXCEPTION: HYPERVISOR INJECTION EXCEPTION/e";
char vmm[]        = "EXCEPTION: VMM COMMUNICATION EXCEPTION/e";
char sec[]        = "EXCEPTION: SECURITY EXCEPTION/e";
char triple[]     = "EXCEPTION: TRIPLE FAULT/e";

__attribute__((interrupt)) void divByZeroISR(struct interrupt_frame* frame){
    screenPrintChars(divByZero);
    while(1);
}
__attribute__((interrupt)) void debugISR(struct interrupt_frame* frame){
    screenPrintChars(debug);
    while(1);
}
__attribute__((interrupt)) void nmiISR(struct interrupt_frame* frame){
    screenPrintChars(nmi);
    while(1);
}
__attribute__((interrupt)) void breakpointISR(struct interrupt_frame* frame){
    screenPrintChars(breakpoint);
    while(1);
}
__attribute__((interrupt)) void overflowISR(struct interrupt_frame* frame){
    screenPrintChars(overflow);
    while(1);
}
__attribute__((interrupt)) void boundISR(struct interrupt_frame* frame){
    screenPrintChars(bound);
    while(1);
}
__attribute__((interrupt)) void opcodeISR(struct interrupt_frame* frame){
    screenPrintChars(opcode);
    while(1);
}
__attribute__((interrupt)) void deviceISR(struct interrupt_frame* frame){
    screenPrintChars(device);
    while(1);
}
__attribute__((interrupt)) void doubleF_ISR(struct interrupt_frame* frame){
    screenPrintChars(doubleF);
    while(1);
}
__attribute__((interrupt)) void TSS_ISR(struct interrupt_frame* frame){
    screenPrintChars(TSS);
    while(1);
}
__attribute__((interrupt)) void segPresISR(struct interrupt_frame* frame){
    screenPrintChars(segPres);
    while(1);
}
__attribute__((interrupt)) void segStackISR(struct interrupt_frame* frame){
    screenPrintChars(segStack);
    while(1);
}
__attribute__((interrupt)) void gpfISR(struct interrupt_frame* frame){
    screenPrintChars(gpf);
    while(1);
}
__attribute__((interrupt)) void pfISR(struct interrupt_frame* frame){
    screenPrintChars(pf);
    while(1);
}
__attribute__((interrupt)) void x87ISR(struct interrupt_frame* frame){
    screenPrintChars(x87);
    while(1);
}
__attribute__((interrupt)) void alignmentISR(struct interrupt_frame* frame){
    screenPrintChars(alignment);
    while(1);
}
__attribute__((interrupt)) void machineISR(struct interrupt_frame* frame){
    screenPrintChars(machine);
    while(1);
}
__attribute__((interrupt)) void simdISR(struct interrupt_frame* frame){
    screenPrintChars(simd);
    while(1);
}
__attribute__((interrupt)) void virtISR(struct interrupt_frame* frame){
    screenPrintChars(virt);
    while(1);
}
__attribute__((interrupt)) void crtISR(struct interrupt_frame* frame){
    screenPrintChars(crt);
    while(1);
}
__attribute__((interrupt)) void hyperISR(struct interrupt_frame* frame){
    screenPrintChars(hyper);
    while(1);
}
__attribute__((interrupt)) void vmmISR(struct interrupt_frame* frame){
    screenPrintChars(vmm);
    while(1);
}
__attribute__((interrupt)) void secISR(struct interrupt_frame* frame){
    screenPrintChars(sec);
    while(1);
}