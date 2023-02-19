;https://wiki.osdev.org/Setting_Up_Long_Mode
;https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
[org 0x8000]
[bits 16]
    ;kill cursor
    mov ah, 0x01
	mov cx, 0x2607
    int 0x10

memorymap:  
    mov ax, 0
    mov es, ax
    mov edx, 0x534D4150
    mov eax, 0x0000E820
    mov ecx, 20
    mov dword[MEMORYMAP_CNT], 0
    mov edi, MEMORYMAP_START
    xor ebx, ebx
    int 0x15
    jc reboot
.l1:
    add edi, 20
    inc dword [MEMORYMAP_CNT]
    test ebx, ebx 
    jz .exit
    mov eax, 0xE820
    mov edx, 0x534d4150
    mov ecx, 20
    int 0x15
    jnc .l1

.exit:
    jmp PM_enter

	db "THIS IS THE KERNEL MADE BY LEON"

%define PM_STACK 0x8000
PM_enter:
	cli
	lgdt[gdtrbuffer]
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax	
	jmp CODE_SEG:PM_enter_2
[bits 32]
PM_enter_2:
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ebp, PM_STACK
	mov esp, ebp
	jmp start
;------------------------------------------------------------------------------------------------------------------------------

start:
    ;check if LM is supported
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb reboot
    mov eax, 0x80000001
    cpuid
    test edx, (1 << 29)
    jz reboot
    ;setup PML4 table
    mov edi, PML4_BASE
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd
    mov edi, cr3
    mov dword [edi], 0x71003
    add edi, 0x1000
    mov dword [edi], 0x72003
    add edi, 0x1000
    mov dword [edi], 0x73003
    ;setup PT
    add edi, 0x1000
    mov ebx, 0x00000003
    mov ecx, 512
    loop1:
    mov dword [edi], ebx
    add ebx, 0x1000
    add edi, 8
    loop loop1
    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax
    ;enter LM
    mov eax, (CR4_PAE | CR4_PGE) 
    mov cr4, eax
    mov eax, PML4_BASE
    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, EFER_LME
    wrmsr
    mov ebx, cr0
    or ebx, (CR0_PG)
    mov cr0, ebx
    ;setup GDT
    lgdt [GDT64.Pointer] 
    mov ax, GDT64.Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp GDT64.Code:kernel64

reboot:
    lidt[rebootidt]
rebootidt:
    dw 0
    dd 0
;-------------------------------------------------------------------------------------------
PML4_BASE equ 0x70000
CR0_PE    equ 1 << 0
CR0_PG    equ 1 << 31
CR4_PAE   equ 1 << 5
CR4_PGE   equ 1 << 7
EFER_LME  equ 1 << 8
;---GDT-TO-ENTER-PM---
gdt_start:
gdt_null:
    dd 0
	dd 0
gdt_code:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10011010b
	db 11001111b
	db 0x00
gdt_data:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 10010010b
	db 11001111b
	db 0x00
gdt_end:
gdtrbuffer:
	dw gdt_end - gdt_start - 1
	dd gdt_start
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
;---GDT-TO-ENTER-LM---
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0
GRAN_4K        equ 1 << 7
SZ_32          equ 1 << 6
LONG_MODE      equ 1 << 5
GDT64:
    .Null: equ $ - GDT64
        dq 0
    .Code: equ $ - GDT64
        dd 0xFFFF
        db 0
        db PRESENT | NOT_SYS | EXEC | RW
        db GRAN_4K | LONG_MODE | 0xF
        db 0
    .Data: equ $ - GDT64
        dd 0xFFFF
        db 0
        db PRESENT | NOT_SYS | RW
        db GRAN_4K | SZ_32 | 0xF
        db 0
    .Pointer:
        dw $ - GDT64 - 1
        dq GDT64
;---------------------------------------------------------
kernelstart equ 0x80000000
V_stackstart  equ 0x0000400000000000
P_stackstart  equ 0x0000000040001000
;---------------------------------------------------------
;MEMORYMAP
align 0x1000
MEMORYMAP_CNT: dw 0
MEMORYMAP_START: db 0
align 0x1000
;KERNEL enter
[bits 64]
kernel64:
;REMAP KERNEL TO TARGET POSITION
;;set PML4 entry

;;xor rdi, rdi
;;mov rdi, PML4_BASE
;mov QWORD [rdi + KERNEL_START_PML4], (MAP_PDPT_ADDR | 0x03)
;;set PDPT entry
;;mov rdi, MAP_PDPT_ADDR
;;mov QWORD [rdi + KERNEL_START_PDPT], (MAP_PD_ADDR | 0x03)
;;set PD entry
mov rdi, 0x72000
mov QWORD [rdi + KERNEL_START_PD], (MAP_PT_ADDR | 0x03)
;;load page table
mov rcx, 512
mov rax, kernel_load
mov rbx, MAP_PT_ADDR
;
.loop1:
    mov      [rbx], rax
    mov BYTE [rbx], 0x03
    ;-
    add rax, 0x1000
    add rbx, 8
    ;
    loop .loop1
jmp KERNEL_START
;loading address of kernel
align 0x1000
kernel_load:




;hast to be pt aligned so 0xE000000 aligned
KERNEL_START      equ 0xFE00000

KERNEL_START_PML4 equ ((KERNEL_START & 0x0000FF8000000000) >> (47)) * 8 
KERNEL_START_PDPT equ ((KERNEL_START & 0x0000007FC0000000) >> (38)) * 8
KERNEL_START_PD   equ ((KERNEL_START & 0x000000003FE00000) >> (21)) * 8

MAP_PDPT_ADDR equ 0x74000
MAP_PD_ADDR   equ 0x75000
MAP_PT_ADDR   equ 0x76000
