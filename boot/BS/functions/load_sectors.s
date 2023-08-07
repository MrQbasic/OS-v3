; uses BIOS int 13h to read from disk
; al - number of sectors
; ebx - load address base
; dh - head (0 based)
; dl - drive
; ch - cylinder (0 based)
; cl - sector to begin reading from (1 based)
[bits 16]
load_sectors:
	push ax		  ;save regs
	push bx
	push cx
	push dx
	push esi

	mov si, ax

	and ebx, 0x000F_FFFF  ;limit addr space adressable in 16 bit mode	
	ror ebx, 12
	shl bx, 8
	mov es, bx
	xor bx, bx
	rol ebx, 12

.reset:
	mov ah, 0	  ; reset disk
	int 0x13
	jc .reset	  ; reset again if error

	mov ah, 0x02  ; BIOS read
	int 0x13
	jc read_error_2 ; carry flag is set on error

	mov cx, si
	cmp al, cl
	jne read_error_1

	pop esi
	pop dx
	pop cx
	pop bx
	pop ax
	ret

read_error_1:
	mov esi, -1  ;SIGNAL ERROR
read_error_2:
	mov ah, 0x0A
	mov al, "E"
	int 0x10
	jmp $