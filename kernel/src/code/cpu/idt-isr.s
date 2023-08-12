extern pic_eoi
extern screenPrintChars

SECTION .data
    ist_isr_default_msg: db "INT! /n/e"

SECTION .text
    GLOBAL idt_isr_default

idt_isr_default:
    push rax

    mov rax, 0xB8000
    inc WORD [rax]

    mov al, 0x20
    out 0x20, al
    
    pop rax
    iretq