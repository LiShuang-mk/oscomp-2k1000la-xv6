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
#include <hsai_global.hh>
#include <device_manager.hh>

namespace dev
{
	Console k_console;

	Console::Console( const char *name )
	{
		_lock.init( name );
		_debug_uart = ( hsai::VirtualUartController * ) hsai::k_devm.get_char_device( name );
	}

	void Console::putc( char c )
	{
		if ( _debug_uart )
		{
			if ( c == _backspace )
			{
				_debug_uart->put_char_sync( '\b' );
				_debug_uart->put_char_sync( ' ' );
				_debug_uart->put_char_sync( '\b' );
			}
			else
				_debug_uart->put_char_sync( c );
		}
	}

	void Console::handle_uart_intr()
	{
		_debug_uart->handle_intr();
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
			_debug_uart->put_char( c );
		}
		return i;
	}

} // namespace dev
