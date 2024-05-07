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
	push ebx
	push cx
	push dx
	push esi

	;save sectors to read cnt for later checkup
	mov si, ax	
	
	;do address calculation ebx->es:bx
	ror ebx, 4
	mov es, bx
	shr ebx, 28


	;setup cnt for reset loop
	push esi
	mov esi, 0xFF
.reset:
	;check for reset cnt limit
	dec esi
	jz read_error_3
	;reset disk
	mov ah, 0
	int 0x13
	;if it doesn't work try again
	jc .reset
	pop esi

	;read from disk
	mov ah, 0x02
	int 0x13
	;error out if it didn't work
	jc read_error_2

	;check for sectors read
	mov cx, si
	cmp al, cl
	jne read_error_1

	pop esi
	pop dx
	pop cx
	pop ebx
	pop ax
	ret

read_error_3:
	mov cx, 1
	mov ah, 0x0A
	mov al, "L"
	int 0x10
	jmp $


read_error_1:
	mov esi, -1  ;SIGNAL ERROR
read_error_2:
	pusha
	mov cx, 1
	mov ah, 0x0A
	mov al, "E"
	int 0x10
	popa
	jmp $