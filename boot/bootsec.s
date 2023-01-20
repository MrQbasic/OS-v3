[bits 16]		        ; force 16 bit code for real mode
[org 0x7c00]		    ; set base address
start:
	cli                 ; no interrupts
	xor ax, ax          ; zero out
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, STACK	    ; set stack address
	mov sp, bp
	jmp 0:main	        ; far jump for cs
main:
	sti			        ; enable interrupts
	mov [BOOT_DRV], dl	; remember the boot device
	call load_kernel	; actually load the system
	call KERNEL_OFFSET
	hlt
	jmp $

KERNEL_OFFSET equ 0x1000
STACK equ 0x9000

BOOT_DRV db 0

%include "./functions/load_sectors.s"
%include "./functions/load_kernel.s"

times 510-($-$$) db 0	; padding
dw 0xaa55		        ; bootable word
