/* Entry Point */
ENTRY(Reset_Handler)

__mcu_flash_size = 64K;
__mcu_ram_size = 20K;

__mcu_flash_page_size = 1K;
__mcu_flash_eeprom_pages = 1;


/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0;      /* required amount of heap  */
_Min_Stack_Size = 0x100; /* required amount of stack */

__mcu_flash_rom_offset = 0x08000000;
__mcu_ram_offset = 0x20000000;

__mcu_flash_eeprom_size = __mcu_flash_eeprom_pages * __mcu_flash_page_size;
__mcu_flash_rom_size = __mcu_flash_size - __mcu_flash_eeprom_size;
__mcu_flash_eeprom_offset = __mcu_flash_rom_offset + __mcu_flash_rom_size;

/* Highest address of the user mode stack */
_estack = __mcu_ram_offset + __mcu_ram_size;    /* end of RAM */

/* Specify the memory areas */
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);



/* Section Definitions */ 
SECTIONS
{
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    _sidata = LOADADDR(.data);

    .data : 
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM AT> FLASH

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        . = ALIGN(4);
        _ebss = .;
    } >RAM
}