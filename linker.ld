SECTIONS
{
    . = 0xFE00000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .end : { *(.end) }
}