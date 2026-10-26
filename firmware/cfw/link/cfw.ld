/*
 * cfw.ld — GNU ld linker script for the DSO152 CFW skeleton.
 *
 * CH32F103C8T6 memory layout:
 *   FLASH: 0x08000000 .. 0x0800FFFF  (64 KB)
 *   SRAM : 0x20000000 .. 0x20001FFF  (8 KB)
 *
 * Boot: the vector table lives at 0x08000000 (.cfv_vector_table).
 * _estack / _estack_start bound the 256-byte stack at the top of SRAM.
 */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 8K
}

ENTRY(Reset_Handler)

SECTIONS
{
  /* Vector table at the very start of FLASH (CPU boots here). */
  .cfv_vector_table : {
    . = ALIGN(4);
    *(.cfv_vector_table)
    . = ALIGN(4);
  } > FLASH

  .text : {
    . = ALIGN(4);
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  /* Initialized data: loaded into FLASH, copied to SRAM at boot. */
  .data : {
    . = ALIGN(4);
    _data_load_start = LOADADDR(.data);
    _data_start = .;
    *(.data*)
    . = ALIGN(4);
    _data_end = .;
  } > SRAM AT> FLASH

  /* Uninitialized data (BSS): zeroed at boot. */
  .bss : {
    . = ALIGN(4);
    _bss_start = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _bss_end = .;
  } > SRAM

  /* 256-byte stack at the top of SRAM. _estack is the initial SP. */
  .stack : {
    . = ALIGN(8);
    . += 256;
    . = ALIGN(8);
    _estack = .;
  } > SRAM

  /DISCARD/ : { *(.ARM.exidx) *(.ARM.attributes) }
}
