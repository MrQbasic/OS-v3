DIR_BUILD   := build
DIR_CODE    := src/code
DIR_INCLUDE := src/include

NASM	   := nasm
NASM_FLAGS := -f elf64
GCC        := gcc
GCC_FLAGS  := -fno-stack-protector -I $(DIR_INCLUDE)/
LD		   := ld
LD_FLAGS   := --no-relax -T linker.ld

FILE_OUT  := kernel.bin

FILES_C_ALL := $(shell find $(DIR_CODE) -type f \( -name '*.c' -not -wholename '*/kernel.c' \))
FILES_C_ISR := $(shell find $(DIR_CODE) -name '*.isr.c')
FILES_C	    := $(DIR_CODE)/kernel.c $(filter-out $(FILES_C_ISR), $(FILES_C_ALL))

FILES_S 	:= $(shell find $(DIR_CODE) -name '*.s')

FILES_O_C	  := $(patsubst $(DIR_CODE)/%.c,$(DIR_BUILD)/%.o,$(FILES_C))
FILES_O_C_ISR := $(patsubst $(DIR_CODE)/%.c,$(DIR_BUILD)/%.o,$(FILES_C_ISR))
FILES_O_S     := $(patsubst $(DIR_CODE)/%.s,$(DIR_BUILD)/%.o,$(FILES_S))
FILES_O       := $(FILES_O_C) $(FILES_O_C_ISR) $(FILES_O_S)

.SILENT: $(FILE_OUT)

#LINK KERNEL
$(FILE_OUT): $(FILES_O)
	$(LD) $(LD_FLAGS) -o $(DIR_BUILD)/kernel.elf $^
	objcopy -O binary -j .text -j .data -j .rodata -j .got -j .got.plt -j .eh_frame -j .bss $(DIR_BUILD)/kernel.elf $@

#COMPILE ISR
$(FILES_O_C_ISR): $(DIR_BUILD)/%.o: $(DIR_CODE)/%.c
	$(GCC) $(GCC_FLAGS) -mgeneral-regs-only -c $^ -o $@
#COMPILE NORMAL
$(FILES_O_C): $(DIR_BUILD)/%.o: $(DIR_CODE)/%.c
	$(GCC) $(GCC_FLAGS) -c $^ -o $@
#COMPILE ASM
$(FILES_O_S): $(DIR_BUILD)/%.o: $(DIR_CODE)/%.s
	$(NASM) $^ $(NASM_FLAGS) -o $@ 