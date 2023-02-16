SECTIONS
{
    . = 0xFE00000;
    ENTRY(main)
    .text : { *(.text) }
    .data : { *(.data) }
    .rodata : { *(.rodata) }
    .got : { *(.got) }
    .got.plt : { *(.got.plt) }
    .eh_frame : { *(.eh_frame) }
    .bss : { *(.bss) }
    .end : { *(.end) }
}