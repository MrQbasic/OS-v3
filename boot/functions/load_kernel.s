[bits 16]			        ; force 16 bit code for real mode
load_kernel:
	pusha
	mov bx, KERNEL_OFFSET	; offset to load the sectors at
	mov al, 50		        ; number of sectors
	mov dl, [BOOT_DRV]	    ; id of the drive
	mov ch, 0		        ; cylinder (0 based)
	mov cl, 2		        ; sector (1 based), skip the MBR and start from sector 2
	mov dh, 0		        ; head one (0 based)
	call load_sectors
	popa
	ret
