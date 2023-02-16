    ; uses BIOS int 13h to read from disk
	; al - number of sectors
	; ebx - load address base
	; dh - head (0 based)
	; dl - drive
	; ch - cylinder (0 based)
	; cl - sector to begin reading from (1 based)
[bits 16]
load_sectors:
	push ax
	push bx
	push cx
	push dx
	push ax
.reset:
	mov ah, 0	  ; reset disk
	int 0x13
	jc .reset	  ; reset again if error
	mov ah, 0x02  ; BIOS read
	int 0x13
	jc read_error ; carry flag is set on error
	pop cx		  ; pop requested number of sectors
	cmp al, cl	  ; al - number of read sectors returned
	jne read_error
	pop dx
	pop cx
	pop bx
	pop ax
	ret

read_error:
	jmp $