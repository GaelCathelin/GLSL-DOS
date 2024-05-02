SECTIONS
{
    . = 0x0100;
    .text : ALIGN(0x1)
    {
        *(.text.startup);
        *(.text);
    }
    .data : ALIGN(0x1)
    {
        *(.rodata);
    }
}