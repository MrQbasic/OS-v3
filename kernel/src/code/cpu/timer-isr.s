extern pi

SECTION .data
    global syscounter_cnt
    syscounter_cnt: dq 0
    chars:          db "--\\||//"

SECTION .text
    global syscounter_isr

syscounter_isr:
    cli
    push rax
    push rbx
    ;inc counter
    inc QWORD [syscounter_cnt]
    mov rax, [syscounter_cnt]   
    and rax, 0b111
    ;get char (base + cnt as offset)
    mov rbx, chars
    add rbx, rax
    mov bl, [rbx]
    ;write char
    mov rax, 0xB8000 + 2*79
    
    mov [rax], bl 

    pop rbx

    mov al, 0x20
    out 0x20, al
    pop rax
    iretq
