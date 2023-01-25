SECTIONS
{
    . = 0xEF00000;
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
    .end : { *(.end) }
}