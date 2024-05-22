OUTPUT_ARCH( "loongarch" )
ENTRY( _entry )

SECTIONS
{
  . = 0x9000000090000000;

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

	. = ALIGN(0x1000);
	_start_u_init = .;

	_u_init_stks = .;
	*(.user.init.stack);
	_u_init_stke = .;

	. = ALIGN(0x1000);
	_u_init_txts = .;
	*(.user.init);
	_u_init_txte = .;

	. = ALIGN(0x1000);
	_u_init_dats = .;
	*(.user.init.data);
	_u_init_date = .;

	_end_u_init = .;
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
