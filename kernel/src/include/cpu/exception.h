struct interrupt_frame;

#define EXC_IDT_FLAG 0x8e

__attribute__((interrupt)) void divByZeroISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void debugISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void nmiISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void breakpointISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void overflowISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void boundISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void opcodeISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void deviceISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void doubleF_ISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void TSS_ISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void segPresISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void segStackISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void gpfISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void pfISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void x87ISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void alignmentISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void machineISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void simdISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void virtISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void crtISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void hyperISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void vmmISR(struct interrupt_frame* frame);
__attribute__((interrupt)) void secISR(struct interrupt_frame* frame);