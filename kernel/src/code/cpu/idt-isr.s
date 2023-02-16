extern pic_eoi
extern screenPrintChars

SECTION .data
    ist_isr_default_msg: db "INT! /n/e"


SECTION .text
    GLOBAL idt_isr_default

idt_isr_default:
    mov rdi, ist_isr_default_msg
    call screenPrintChars
    call pic_eoi
    iretq 