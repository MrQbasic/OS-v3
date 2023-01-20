SECTION .end
    align 0x1000
    global endptr
    endptr:
SECTION .text
    global endptr_getptr
endptr_getptr:
    mov rax, endptr
    ret