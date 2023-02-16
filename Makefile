DIR_KERNEL := kernel
DIR_BOOT   := boot

FILE_BOOT_BIN   := $(DIR_BOOT)/boot.bin
FILE_KERNEL_BIN := $(DIR_KERNEL)/kernel.bin

FILE_OUT := OS.img

.SILENT: $(FILE_OUT) $(FILE_BOOT_BIN) $(FILE_KERNEL_BIN)

#CREATE FINAL IMAGE
$(FILE_OUT): $(FILE_BOOT_BIN) $(FILE_KERNEL_BIN)
	cat $^ 1,44mb.img > tmp.img
	dd if=tmp.img of=$@ bs=512 count=2880
	rm tmp.img
	echo ""
	ls -lha OS.img
	ls -lha $(FILE_BOOT_BIN)
	ls -lha $(FILE_KERNEL_BIN)


$(FILE_BOOT_BIN):
	cd $(DIR_BOOT); make

$(FILE_KERNEL_BIN):
	cd $(DIR_KERNEL); make

clean:
	rm -f log.txt
	rm -f OS.img
	rm -f $(shell find . -type f \( -name "*.o" -or -name "*.bin" -or -name "*.elf" \) )
	clear

run: $(FILE_OUT)
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