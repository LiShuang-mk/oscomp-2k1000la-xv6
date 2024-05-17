OUTPUT_ARCH( "loongarch" )
ENTRY( _entry )

SECTIONS
{
  . = 0x9000000080000000;

  .text : {
	PROVIDE(stext = .);
    *(.text .text.*)
    . = ALIGN(0x1000);
    _keentry = .;
    *(k_eentry)
    . = ALIGN(0x1000);
    _ueentry = .;
    *(u_eentry)
    . = ALIGN(0x1000);
    _tlbrentry = .;
    *(tlbrentry)
    . = ALIGN(0x1000);
    _merrentry = .;
    *(merrentry)
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
	PROVIDE(_start_u_init = .);
	*(.user.init);
	PROVIDE(_end_u_init = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*)
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) 
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.sbss .sbss.*) 
    . = ALIGN(16);
    *(.bss .bss.*)
  }

  PROVIDE(end = .);
}
