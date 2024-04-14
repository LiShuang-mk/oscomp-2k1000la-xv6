//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/printer.hh"
#include "klib/common.hh"
#include "console.hh"

namespace kernellib
{
	Printer k_printer;

	int Printer::_trace_flag = 0;
	char Printer::_digits[] = "0123456789abcdef";

	void Printer::init( Console *console, const char *name )
	{
		_type = out_type::console;
		_console = console;
		_lock.init( name );
		_locking = 1;
		_panicked = 0;
	}

	void Printer::printint( int xx, int base, int sign )
	{
		char buf[ 16 ];
		int i;
		uint x;

		if ( sign && ( sign = xx < 0 ) )
			x = -xx;
		else
			x = xx;

		i = 0;
		do
		{
			buf[ i++ ] = _digits[ x % base ];
		}
		while ( ( x /= base ) != 0 );

		if ( sign )
			buf[ i++ ] = '-';

		if ( _type == out_type::console && _console )
			while ( --i >= 0 )
				_console->putc( buf[ i ] );
	}

	void Printer::printptr( uint64 x )
	{
		if ( _type != out_type::console || _console == nullptr )
			return;
		uint64 i;
		_console->putc( '0' );
		_console->putc( 'x' );
		for ( i = 0; i < ( sizeof( uint64 ) * 2 ); i++, x <<= 4 )
			_console->putc( _digits[ x >> ( sizeof( uint64 ) * 8 - 4 ) ] );
	}

	void Printer::printf( const char *fmt, ... )
	{
		va_list ap;
		va_start( ap, fmt );
		vprintf( fmt, ap );
		va_end( ap );
	}

	void Printer::vprintf( const char *fmt, va_list ap )
	{
		int i, c;
		int locking;
		char *s;

		locking = _locking;
		if ( locking )
			_lock.acquire();

		if ( fmt == 0 )
			log_panic( "null fmt" );

		for ( i = 0; ( c = fmt[ i ] & 0xff ) != 0; i++ )
		{
			if ( c != '%' )
			{
				if ( _type == out_type::console && _console )
				{
					if ( _trace_flag == 1 && c == '\n' )
					{
						_locking = 0;
						_trace_flag = 0;
						printf( "\n\t     " );
						_trace_flag = 1;
						_locking = 1;
					}
					else
					{
						_console->putc( c );
					}
				}
				continue;
			}
			c = fmt[ ++i ] & 0xff;
			if ( c == 0 )
				break;
			switch ( c )
			{
				case 'd':
					printint( va_arg( ap, int ), 10, 1 );
					break;
				case 'x':
					printint( va_arg( ap, int ), 16, 0 );
					break;
				case 'p':
					printptr( va_arg( ap, uint64 ) );
					break;
				case 's':
					if ( ( s = va_arg( ap, char* ) ) == 0 )
						s = ( char * ) "(null)";
					if ( _type == out_type::console && _console )
						for ( ; *s; s++ )
							_console->putc( *s );
					break;
				case '%':
					if ( _type == out_type::console && _console )
						_console->putc( '%' );
					break;
				default:
				  // Print unknown % sequence to draw attention.
					if ( _type == out_type::console && _console )
					{
						_console->putc( '%' );
						_console->putc( c );
					}
					break;
			}
		}

		if ( locking )
			_lock.release();
	}

	void Printer::panic( const char *f, uint l, const char *info, ... )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[31m[ panic ] => " );
#else 
		k_printer.printf( "[ panic ] => " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.printf( "%s", info );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._panicked = 1;
		while ( 1 );
	}

	void Printer::error( const char *f, uint l, const char *info, ... )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[35m[ error ] => " );
#else 
		k_printer.printf( "[ error ] => " );
#endif  
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.printf( "%s", info );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::warn( const char *f, uint l, const char *info, ... )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[33m[ warn ]  => " );
#else 
		k_printer.printf( "[ warn ]  => " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.printf( "%s", info );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::info( const char *f, uint l, const char *info, ... )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[36m[ info ]  => " );
#else 
		k_printer.printf( "[ info ]  => " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.printf( "%s", info );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::trace( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[93m[ trace ] => " );
#else 
		k_printer.printf( "[ trace ] => " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::assrt( const char *f, uint l, const char *expr )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[91m[ assert ]=> " );
#else 
		k_printer.printf( "[ assert ]=> " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.printf( expr );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\n\033[0m" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;

		panic( f, l, "assert fail for above reason." );
	}
}
