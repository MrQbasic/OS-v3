SECTIONS
{
    . = 0x3000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .end : { *(.end) }
}