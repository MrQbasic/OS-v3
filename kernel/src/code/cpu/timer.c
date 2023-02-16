#include "cpu/idt.h"
#include "cpu/pic.h"
#include "cpu/timer.h"
#include "screen.h"

uint64_t syscounter_get(){
    extern uint64_t syscounter_cnt;
    return syscounter_cnt;
}


void syscounter_init(){
    extern void syscounter_isr();
    idt_set((uint64_t) syscounter_isr, SYSCOUNTER_SEGSEL, SYSCOUNTER_IDTE, SYSCOUNTER_FLAG);
}
