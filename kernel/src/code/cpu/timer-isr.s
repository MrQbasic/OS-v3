extern pi

SECTION .data
    global syscounter_cnt
    syscounter_cnt: dq 0

SECTION .text
    global syscounter_isr

syscounter_isr:
    cli
    push rax   
    inc QWORD [syscounter_cnt]
    mov al, 0x20
    out 0x20, al
    pop rax
    iretq
