//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/console.hh"
#include "mm/virtual_memory_manager.hh"

#include <uart/virtual_uart.hh>

namespace dev
{
	Console k_console;

	Console::Console() {}

	void Console::init( const char *name, bool use_default_uart )
	{
		_lock.init( name );
		// _uart_m.init( loongarch::qemuls2k::InterAddr::uart0 );
		_use_k_debug_uart = use_default_uart;
	}

	void Console::putc( char c )
	{
		if ( hsai::k_debug_uart && _use_k_debug_uart )
		{
			if ( c == _backspace )
			{
				hsai::k_debug_uart->put_char_sync( '\b' );
				hsai::k_debug_uart->put_char_sync( ' ' );
				hsai::k_debug_uart->put_char_sync( '\b' );
			}
			else
				hsai::k_debug_uart->put_char_sync( c );
		}
	}

	void Console::handle_uart_intr()
	{
		hsai::k_debug_uart->handle_interrupt();
	}

	int Console::write( int from_user, uint64 src, uint64 n )
	{
		char c;
		uint64 i;
		bool user_src = from_user == 1 ? true : false;
		for ( i = 0; i < n; ++i )
		{
			if ( mm::k_vmm.either_copy_in( &c, user_src, src + i, 1 ) < 0 )
				break;
			hsai::k_debug_uart->put_char( c );
		}
		return i;
	}

} // namespace dev
