NASM	   := nasm
NASM_FLAGS := -f bin

FILE_OUT := boot.bin
PM_OUT := PM/kernel-PM.bin
BS_OUT := BS/bootsec.bin

.SILENT: $(FILE_OUT)

$(FILE_OUT): $(BS_OUT) $(PM_OUT)
	cat $^ > $@

$(PM_OUT):
	cd PM; $(NASM) $(NASM_FLAGS) kernel-PM.s -o kernel-PM.bin

$(BS_OUT):
	cd BS; $(NASM) $(NASM_FLAGS) bootsec.s -o bootsec.bin
