//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/console.hh"
#include "hal/qemu_ls2k.hh"

namespace dev
{

	Console k_console;

	Console::Console() {}

	void Console::init( const char *name )
	{
		_lock.init( name );
		_uart_m.init( loongarch::qemuls2k::InterAddr::uart0 );
	}

	void Console::putc( char c )
	{
		if ( c == _backspace )
		{
			_uart_m.putc_sync( '\b' );
			_uart_m.putc_sync( ' ' );
			_uart_m.putc_sync( '\b' );
		}
		else
			_uart_m.putc_sync( c );
	}

	void Console::handle_uart_intr()
	{
		_uart_m.read_lsr();
		_uart_m.read_rhr();
	}

} // namespace dev
