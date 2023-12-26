extern pic_eoi
extern screenPrintChars

SECTION .text
    GLOBAL idt_isr_default

idt_isr_default:
    mov rcx, 80*20
    mov rbx, 0xB8000
.fill_loop1:
    mov BYTE [rbx+1], 0x40
    add rbx, 2
    loop .fill_loop1

    jmp $