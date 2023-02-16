SECTION .data
    global syscounter_cnt
    asdf: dq 0
    syscounter_cnt: dq 0x1234

SECTION .text
    global syscounter_isr

syscounter_isr:
    cli
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi

    mov rdi, syscounter_cnt
    inc QWORD [rdi]

    mov al, 0x20
    out 0x20, al
    out 0xA0, al

    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq