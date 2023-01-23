BIN_BOOT  := boot/bootsec.bin
ASM_BOOT  := boot/bootsec.s

S_KERNEL_PM := kernel/PM/kernel-PM.s
BIN_KERNEL_PM := kernel/PM/kernel-PM.bin

BIN_KERNEL   := kernel/kernel.bin

C_KERNEL_ISR := $(shell find kernel/lib/code -name '*.isr.c')

C_KERNEL_TMP := kernel/kernel.c $(shell find kernel/lib/ -name '*.c')
C_KERNEL     := $(filter-out $(C_KERNEL_ISR), $(C_KERNEL_TMP)) 

S_KERNEL     := $(shell find kernel/lib/code/ -name '*.s')

O_C_KERNEL   := $(patsubst kernel/%.c,kernel/%.o,$(C_KERNEL)) 
O_ISR_KERNEL := $(patsubst kernel/%.c,kernel/%.o,$(C_KERNEL_ISR))
O_S_KERNEL   := $(patsubst kernel/%.s,kernel/%.o,$(S_KERNEL))

H_KERNEL     := $(shell find kernel/lib/include/ -name '*.h')



#put everyting together
OS.img: $(BIN_BOOT) $(BIN_KERNEL_PM) $(BIN_KERNEL)
	cat $^ 1,44mb.img > tmp.img
	dd if=tmp.img of=$@

#compile bootloader
$(BIN_BOOT): $(ASM_BOOT)
	cd ./boot; nasm bootsec.s -f bin -o bootsec.bin

#compile kernel-pm -> obj
$(BIN_KERNEL_PM): $(S_KERNEL_PM)
	cd ./kernel/PM; nasm kernel-PM.s -f bin -o kernel-PM.bin

#compile kernel
$(BIN_KERNEL): $(O_C_KERNEL) $(O_S_KERNEL) $(O_ISR_KERNEL)
	ld -o kernel/kernel.elf -T linker.ld $^
	objcopy -O binary -j .text -j .data -j .bss kernel/kernel.elf $@

$(O_C_KERNEL): %.o: %.c
	gcc -c $^ -o $@
	
$(O_S_KERNEL): %.o: %.s
	nasm $^ -f elf64 -o $@ 

$(O_ISR_KERNEL): %.o: %.c
	gcc -mgeneral-regs-only -c $^ -o $@


clean:
	rm -f kernel/PM/kernel-PM.bin
	rm -f kernel/kernel.bin
	rm -f kernel/kernel.elf
	rm -f log.txt
	rm -f tmp.img
	rm -f $(shell find ./kernel/ -type f -name "*.o")
	clear

run:
	rm -f ./log.txt
	clear
	qemu-system-x86_64 \
    -drive id=disk,file=OS.img,if=none,index=0,media=disk,format=raw \
    -device ahci,id=ahci \
    -device ide-hd,drive=disk,bus=ahci.0 \
    -cpu max \
    -D log.txt \
    -d int \
    -m 4G