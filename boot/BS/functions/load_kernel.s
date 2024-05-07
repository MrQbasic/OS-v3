[bits 16]			        ; force 16 bit code for real mode
load_kernel:
	pusha

	;phy    - kernel space
	;0x8000 - 0xFE00000
	mov ebx, KERNEL_OFFSET	; offset to load the sectors at
	mov al, 63		        ; number of sectors
	mov dl, [BOOT_DRV]	    ; id of the drive
	mov ch, 0		        ; cylinder (0 based)
	mov cl, 2		        ; sector (1 based), skip the MBR and start from sector 2
	mov dh, 0		        ; head one (0 based)
	call load_sectors
	
	;FC00 - 0xFE02C00
	mov ebx, KERNEL_OFFSET + (512 * 63) - 512
	mov al, 64
	mov dl, [BOOT_DRV]
	mov ch, 0
	mov cl, 1
	mov dh, 1
	call load_sectors

	;17A00 - 0xFE0AA00
	mov ebx, KERNEL_OFFSET + (512 * 63) + (512 * 64) - (512*2)
	mov al, 64
	mov dl, [BOOT_DRV]
	mov ch, 0
	mov cl, 1
	mov dh, 2
	call load_sectors

	;1F800 - 0xFE12800
	mov ebx, KERNEL_OFFSET + (512 * 63) + (512 * 64) + (512 * 64) - (512*3)
	mov al, 64
	mov dl, [BOOT_DRV]
	mov ch, 0
	mov cl, 1
	mov dh, 3
	call load_sectors

	popa
	ret


