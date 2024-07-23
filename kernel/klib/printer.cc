//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/printer.hh"
#include "klib/common.hh"
#include "fs/dev/console.hh"

#include <hsai_log.hh>

namespace kernellib
{
	Printer k_printer;

	int Printer::_trace_flag = 0;
	char Printer::_digits[] = "0123456789abcdef";

	void Printer::init( dev::Console *console, const char *name )
	{
		_type = out_type::console;
		_console = console;
		_lock.init( name );
		_locking = 1;
		_panicked = 0;

		hsai::p_hsai_logout = &level_log_out;
		hsai::p_hsai_assert = &assert_log_out;
		hsai::p_hsai_printf = &printf_log_out;
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


	void Printer::printbyte( uchar x )
	{
		char buf[ 2 ];

		buf[ 0 ] = _digits[ x & 0xFU ];
		x >>= 4;
		buf[ 1 ] = _digits[ x & 0xFU ];

		if ( _type == out_type::console && _console )
		{
			_console->putc( buf[ 1 ] );
			_console->putc( buf[ 0 ] );
		}
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
						_locking = locking;
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
					printint( va_arg( ap, long ), 10, 1 );
					break;
				case 'u':
					printint( va_arg( ap, long ), 10, 0 );
					break;
				case 'x':
					printint( va_arg( ap, long ), 16, 0 );
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
				case 'B':
					printbyte( ( uchar ) va_arg( ap, int ) );
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
		va_list ap;
		va_start( ap, info );
		k_printer.vprintf( info, ap );
		va_end( ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
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
		va_list ap;
		va_start( ap, info );
		k_printer.vprintf( info, ap );
		va_end( ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
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
		va_list ap;
		va_start( ap, info );
		k_printer.vprintf( info, ap );
		va_end( ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
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
		va_list ap;
		va_start( ap, info );
		k_printer.vprintf( info, ap );
		va_end( ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::trace( const char *f, uint l, const char *info, ... )
	{
		k_printer._locking = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[32m[ trace ] => " );
#else 
		k_printer.printf( "[ trace ] => " );
#endif 
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		va_list ap;
		va_start( ap, info );
		k_printer.vprintf( info, ap );
		va_end( ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;
	}

	void Printer::assrt( const char *f, uint l, const char *expr, const char *detail, ... )
	{
		va_list ap;
		va_start( ap, detail );
		assrt_va( f, l, expr, detail, ap );
		va_end( ap );
	}

	void Printer::assrt_va( const char *f, uint l, const char *expr, const char *detail, va_list ap )
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
		k_printer.printf( "assert fail for '" );
		k_printer.printf( expr );
		k_printer.printf( "'\n[detail] " );
		k_printer.vprintf( detail, ap );
		_trace_flag = 0;
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
#else 
		k_printer.printf( "\n" );
#endif 
		k_printer._locking = 1;

		panic( f, l, "assert fail for above reason." );
	}


	void Printer::log_output_info( const char * f, uint l, const char * info, va_list ap )
	{
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
	}

	void Printer::log_out_va( OutputLevel level, const char * f, uint l, const char * info, va_list ap )
	{

		switch ( level )
		{
			case OutputLevel::out_trace:
			{
#ifdef LINUX_BUILD
				k_printer.printf( "\033[32m[ trace ] => " );
#else 
				k_printer.printf( "[ trace ] => " );
#endif 
			} break;
			case OutputLevel::out_info:
			{
#ifdef LINUX_BUILD
				k_printer.printf( "\033[36m[ info ]  => " );
#else 
				k_printer.printf( "[ info ]  => " );
#endif 
			} break;
			case OutputLevel::out_warn:
			{
#ifdef LINUX_BUILD
				k_printer.printf( "\033[33m[ warn ]  => " );
#else 
				k_printer.printf( "[ warn ]  => " );
#endif 
			} break;
			case OutputLevel::out_error:
			{
#ifdef LINUX_BUILD
				k_printer.printf( "\033[35m[ error ] => " );
#else 
				k_printer.printf( "[ error ] => " );
#endif  
			} break;
			case OutputLevel::out_panic:
			{
#ifdef LINUX_BUILD
				k_printer.printf( "\033[31m[ panic ] => " );
#else 
				k_printer.printf( "[ panic ] => " );
#endif 
			} break;
			default:
				break;
		}
		log_output_info( f, l, info, ap );
#ifdef LINUX_BUILD
		k_printer.printf( "\033[0m\n" );
#else 
		k_printer.printf( "\n" );
#endif 
	}

	void Printer::log_out( OutputLevel level, const char * f, uint l, const char * info, ... )
	{
		va_list ap;
		va_start( ap, info );
		log_out_va( level, f, l, info, ap );
		va_end( ap );
	}

	void level_log_out( hsai::HsaiLogLevel level, const char * fn, uint ln, const char * info, ... )
	{
		va_list ap;
		va_start( ap, info );
		switch ( level )
		{
			case hsai::log_trace:  k_printer.log_out_va( out_trace, fn, ln, info, ap ); break;
			case hsai::log_info:   k_printer.log_out_va( out_info, fn, ln, info, ap ); break;
			case hsai::log_warn:   k_printer.log_out_va( out_warn, fn, ln, info, ap ); break;
			case hsai::log_error:  k_printer.log_out_va( out_error, fn, ln, info, ap ); break;
			case hsai::log_panic:  k_printer.log_out_va( out_panic, fn, ln, info, ap ); break;
			default: break;
		}
		va_end( ap );
	}

	void assert_log_out( const char *f, uint l, const char *expr, const char *detail, ... )
	{
		va_list ap;
		va_start( ap, detail );
		k_printer.assrt_va( f, l, expr, detail, ap );
		va_end( ap );
	}

	void printf_log_out( const char *fmt, ... )
	{
		va_list ap;
		va_start( ap, fmt );
		k_printer.vprintf( fmt, ap );
		va_end( ap );
	}
}
