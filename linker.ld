SECTIONS
{
    . = 0xFE00000;
    .text : { *(.text) }
    .data : { *(.data) }
    .rodata : { *(.rodata) }
    .bss : { *(.bss) }
    .end : { *(.end) }
}