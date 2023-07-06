ENTRY(_start)
SECTIONS
{
    . = 0x1c000000;  /* Set the desired entry address here */
    .text :
    {
        *(.text)
    }
}
