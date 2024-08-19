//
// Created by Li shuang ( pseudonym ) on 2024-03-27
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "klib/printer.hh"

#include <device_manager.hh>
#include <hsai_global.hh>
#include <hsai_log.hh>
#include <stream_device.hh>

#include "klib/back_trace.hh"
#include "klib/klib.hh"

namespace klib
{
	Printer k_printer;

	int	 Printer::_trace_flag	  = 0;
	char Printer::_lower_digits[] = "0123456789abcdef";
	char Printer::_upper_digits[] = "0123456789ABCDEF";

	void Printer::init( const char *name )
	{
		_type = out_type::console;

		_console = (hsai::StreamDevice *) hsai::k_devm.get_device( "stderr" );
		if ( _console == nullptr )
			while ( 1 );
		if ( _console->type() != hsai::dev_char )
			while ( 1 );
		if ( !_console->support_stream() )
			while ( 1 );

		_lock.init( name );
		_locking  = 1;
		_panicked = 0;

		new ( &_buffer ) PrinterBuffer( _internal_buffer, printer_buffer_size );

		hsai::p_hsai_logout = &level_log_out;
		hsai::p_hsai_assert = &assert_log_out;
		hsai::p_hsai_printf = &printf_log_out;
	}

	void Printer::printint( int xx, int base, int sign )
	{
		char buf[16];
		int	 i;
		uint x;

		if ( sign && ( sign = xx < 0 ) )
			x = -xx;
		else
			x = xx;

		i = 0;
		do {
			buf[i++] = _lower_digits[x % base];
		}
		while ( ( x /= base ) != 0 );

		if ( sign ) buf[i++] = '-';

		if ( _type == out_type::console && _console )
			while ( --i >= 0 ) _console->put_char_sync( buf[i] );
	}


	void Printer::printbyte( uint8 x )
	{
		char buf[2];

		buf[0]	 = _lower_digits[x & 0xFU];
		x	   >>= 4;
		buf[1]	 = _lower_digits[x & 0xFU];

		if ( _type == out_type::console && _console )
		{
			_console->put_char_sync( buf[1] );
			_console->put_char_sync( buf[0] );
		}
	}

	void Printer::printptr( uint64 x )
	{
		if ( _type != out_type::console || _console == nullptr ) return;
		uint64 i;
		_console->put_char_sync( '0' );
		_console->put_char_sync( 'x' );
		for ( i = 0; i < ( sizeof( uint64 ) * 2 ); i++, x <<= 4 )
			_console->put_char_sync( _lower_digits[x >> ( sizeof( uint64 ) * 8 - 4 )] );
	}

	void Printer::print_number( PrinterBuffer &buf, ulong num, int base, print_flag flag,
								char qualifier, int width )
	{
		char  tmp[64] = { 0 };
		int	  tmpi	  = 0;
		char *digits  = nullptr;

		int space_pad; // to calculate the space padding length
		int num_pad;   // to calculate the zero padding length
		space_pad = num_pad = 0;

		// zero padding is confict with left-aign

		if ( flag.left == 1 ) flag.zero_pad = 0;

		// check if use upper-case letter

		digits = ( flag.upper == 1 ) ? _upper_digits : _lower_digits;

		// check sign

		char sign = 0;
		if ( flag.sign == 1 )
		{
			switch ( qualifier )
			{
				case 'h':
					if ( (i16) num < 0 )
					{
						sign = '-';
						num	 = (u16) -num;
					}
					break;
				case 'L':
				case 'l':
					if ( (i64) num < 0 )
					{
						sign = '-';
						num	 = (u64) -num;
					}
					break;
				default:
					if ( (i32) num < 0 )
					{
						sign = '-';
						num	 = (u32) -num;
					}
					break;
			}
			if ( sign != '-' )
			{
				if ( flag.plus == 1 )
					sign = '+';
				else if ( flag.space == 1 )
					sign = ' ';
			}
		}

		space_pad = width;

		// push the number bit into tmp

		if ( num == 0 )
		{
			tmp[tmpi] = '0';
			++tmpi;
		}
		else
			for ( ; num != 0; tmpi++ ) tmp[tmpi] = digits[_divide( num, base )];

		space_pad -= tmpi;

		// // seperation should occupy room

		// int sepr_cnt = 0;
		// if ( flag.seperate == 1 )
		// 	sepr_cnt = ( tmpi - 1 ) / 4;
		// space_pad -= sepr_cnt;

		// prefix should occupy room

		if ( flag.prefix == 1 )
		{
			if ( base == 2 || base == 16 )
				space_pad -= 2;
			else if ( base == 8 )
				space_pad -= 1;
		}

		// the sign should occupy room

		if ( sign != 0 ) space_pad--;

		// if use zero-pad, move space_pad to num_pad

		if ( flag.zero_pad == 1 )
		{
			num_pad	  = space_pad;
			space_pad = 0;
		}

		// to pad if not left-align

		if ( flag.left == 0 )
			for ( ; space_pad > 0; space_pad-- ) buf.put( ' ' );

		// put the sign

		if ( sign != 0 ) buf.put( sign );

		// put the prefix

		if ( flag.prefix == 1 )
		{
			if ( base == 2 )
			{
				buf.put( '0' );
				buf.put( 'b' );
			}
			else if ( base == 8 ) { buf.put( '0' ); }
			else if ( base == 16 )
			{
				buf.put( '0' );
				buf.put( 'x' );
			}
		}

		// to pad zero-num

		for ( ; num_pad > 0; num_pad--, tmpi++ ) tmp[tmpi] = '0';

		// to put number in tmp

		int mod = base == 10 ? 3 : 4;

		tmpi--;
		if ( ( tmpi % mod ) == mod - 1 ) // remove '_' in the front
		{
			buf.put( tmp[tmpi] );
			tmpi--;
		}
		if ( flag.seperate == 1 )
		{
			for ( ; tmpi >= 0; tmpi-- )
			{
				if ( ( tmpi % mod ) == mod - 1 ) buf.put( '_' );
				buf.put( tmp[tmpi] );
			}
		}
		else
			for ( ; tmpi >= 0; tmpi-- ) buf.put( tmp[tmpi] );

		// if left-align, it would be space_pad > 0 here

		for ( ; space_pad > 0; space_pad-- ) buf.put( ' ' );
	}

	void Printer::vbprintf( PrinterBuffer &buf, const char *fmt, va_list args )
	{
		if ( buf.size() <= 0 ) return;

		for ( ; *fmt != 0; ++fmt )
		{
			// put a char
			if ( *fmt != '%' )
			{
				if ( _trace_flag == 1 && *fmt == '\n' )
				{
					const char *lb = "\n\t     ";
					for ( ; *lb != 0; ++lb ) buf.put( *lb );
				}
				else
					buf.put( *fmt );
				continue;
			}


			// collect the flag
			print_flag fl = (print_flag) 0;

			for ( bool get_fl = true; get_fl; )
			{
				++fmt; // skip the first '%'
				switch ( *fmt )
				{
					case '-': fl.left = 1; break;
					case '+': fl.plus = 1; break;
					case ' ': fl.space = 1; break;
					case '#': fl.prefix = 1; break;
					case '0': fl.zero_pad = 1; break;
					case '_': fl.seperate = 1; break;
					default : get_fl = false; break;
				}
			}

			// collect the width
			int width = -1;
			if ( _is_number( *fmt ) )
			{
				int i = 0;
				for ( ; _is_number( *fmt ); fmt++ ) i = i * 10 + _to_number( *fmt );
				width = i;
			}
			else if ( *fmt == '*' ) // the width is in the args
			{
				++fmt;
				width = va_arg( args, int );
				if ( width < 0 )
				{
					width	= -width;
					fl.left = 1;
				}
			}

			// collect the qualifier
			char qualifier = 0;
			if ( *fmt == 'l' || *fmt == 'L' || *fmt == 'h' )
			{
				qualifier = *fmt;
				fmt++;
				if ( qualifier == 'l' && *fmt == 'l' )
				{
					qualifier = 'L';
					++fmt;
				}
			}

			static print_flag Bfl = { .zero_pad = 1,
									  .sign		= 0,
									  .plus		= 0,
									  .space	= 0,
									  .left		= 0,
									  .prefix	= 0,
									  .upper	= 1,
									  .seperate = 0 };

			// collect the base, or put directly if format is 'c','s','p','%'
			int			base = 10;
			char		ch;
			const char *str = nullptr;
			int			str_len;
			switch ( *fmt )
			{
				case 'c':
					if ( fl.left == 0 )				 // padding space
						for ( ; width > 1; width-- ) // leave one room to contain the char
							buf.put( ' ' );
					ch = (char) va_arg( args, int );
					buf.put( ch );
					width--;
					for ( ; width > 0; width-- ) buf.put( ' ' ); // padding space
					continue;

				case 's':
					str = va_arg( args, const char * );
					if ( str == nullptr ) str = "(NULL)";
					str_len = (int) strlen( str );
					if ( width > 0 ) str_len = str_len > width ? width : str_len;

					if ( fl.left == 1 )
						for ( ; str_len < width; width-- ) // padding space
							buf.put( ' ' );

					for ( int i = 0; i < str_len; ++i ) buf.put( str[i] );

					for ( ; str_len < width; width-- ) // padding space
						buf.put( ' ' );

					continue;

				case 'p':
					fl.prefix	= 1;
					fl.zero_pad = 1;
					fl.seperate = 1;
					if ( width < 0 ) width = sizeof( void * ) << 1;
					width += 2; // prefix
					print_number( buf, (ulong) va_arg( args, void * ), 16, fl, qualifier, width );
					continue;

				case '%': buf.put( '%' ); continue;

				case 'B':
					qualifier = 'B';
					print_number( buf, (u8) va_arg( args, uint ), 16, Bfl, qualifier, 2 );
					continue;

				case 'b': base = 2; break;
				case 'o': base = 8; break;
				case 'X': fl.upper = 1;
				case 'x': base = 16; break;
				case 'd':
				case 'i': fl.sign = 1; break;
				case 'u': break;

				default:
					const char *idx = fmt;
					do --fmt;
					while ( *fmt != '%' );
					for ( ; fmt < idx; fmt++ ) buf.put( *fmt );
					continue;
			}

			// print number
			ulong num;

			if ( qualifier == 'l' || qualifier == 'L' )
				num = va_arg( args, ulong );
			else if ( qualifier == 'h' )
			{
				num = (u16) va_arg( args, uint );
				if ( fl.sign ) num = (i16) num;
			}
			else if ( qualifier == 'B' ) { num = (u8) va_arg( args, uint ); }
			else
				num = va_arg( args, uint );

			print_number( buf, num, base, fl, qualifier, width );
		}
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
		int locking;

		_buffer.clear();

		locking = _locking;
		if ( locking ) _lock.acquire();

		if ( fmt == 0 ) log_panic( "null fmt" );

		vbprintf( _buffer, fmt, ap );

		if ( locking ) _lock.release();

		if ( _console != nullptr )
			_console->write( (void *) _buffer.get_content(), _buffer.content_length() );
	}

	void Printer::panic( const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		k_printer.panic_va( f, l, info, ap );
		va_end( ap );
	}

	void Printer::panic_va( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( RED_COLOR_PINRT "[ panic ] => " );
#else
		k_printer.printf( "[ panic ] => " );
#endif
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );

		void *bt_buf_fp[256];
		void *bt_buf_ra[256];

		int	  bt_cnt_fp = 0;

		[[maybe_unused]] int bt_cnt_ra = 0;

		bt_cnt_fp = back_trace_fp( bt_buf_fp, 256 );
		bt_cnt_ra = back_trace_ra( bt_buf_ra, 256 );

		k_printer.printf( "\n\e[7mback-trace    fp    ra\n" );
		for ( int i = 0; i < bt_cnt_fp; ++i )
		{
			k_printer.printf( "[%d] :  %p  %p\n", i, bt_buf_fp[i], bt_buf_ra[i] );
		}

		_trace_flag = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
#else
		k_printer.printf( "\n" );
#endif
		k_printer._panicked = 1;
		while ( 1 );
	}

	void Printer::error( const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		k_printer.error_va( f, l, info, ap );
		va_end( ap );
	}

	void Printer::error_va( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( MAGANTA_COLOR_PRINT "[ error ] => " );
#else
		k_printer.printf( "[ error ] => " );
#endif
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
#else
		k_printer.printf( "\n" );
#endif
		k_printer._locking = 1;
	}

	void Printer::warn( const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		k_printer.warn_va( f, l, info, ap );
		va_end( ap );
	}

	void Printer::warn_va( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( YELLOW_COLOR_PRINT "[ warn ]  => " );
#else
		k_printer.printf( "[ warn ]  => " );
#endif
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
#else
		k_printer.printf( "\n" );
#endif
		k_printer._locking = 1;
	}

	void Printer::info( const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		k_printer.info_va( f, l, info, ap );
		va_end( ap );
	}

	void Printer::info_va( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CYAN_COLOR_PINRT "[ info ]  => " );
#else
		k_printer.printf( "[ info ]  => " );
#endif
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
#else
		k_printer.printf( "\n" );
#endif
		k_printer._locking = 1;
	}

	void Printer::trace( const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		k_printer.trace_va( f, l, info, ap );
		va_end( ap );
	}

	void Printer::trace_va( const char *f, uint l, const char *info, va_list ap )
	{

		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( GREEN_COLOR_PRINT "[ trace ] => " );
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
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
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

	void Printer::assrt_va( const char *f, uint l, const char *expr, const char *detail,
							va_list ap )
	{
		k_printer._locking = 0;
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( BLUE_COLOR_PRINT "[ assert ]=> " );
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
#if LINUX_BUILD && OPEN_COLOR_PRINT
		k_printer.printf( CLEAR_COLOR_PRINT "\n" );
#else
		k_printer.printf( "\n" );
#endif
		k_printer._locking = 1;

		panic( f, l, "assert fail for above reason." );
	}


	void Printer::log_output_info( const char *f, uint l, const char *info, va_list ap )
	{
		k_printer.printf( f );
		k_printer.printf( " : " );
		k_printer.printf( "%d", l );
		k_printer.printf( " :\n\t     " );
		_trace_flag = 1;
		k_printer.vprintf( info, ap );
		_trace_flag = 0;
	}

	void Printer::log_out_va( OutputLevel level, const char *f, uint l, const char *info,
							  va_list ap )
	{

		switch ( level )
		{
			case OutputLevel::out_trace:
			{
				trace_va( f, l, info, ap );
			}
			break;
			case OutputLevel::out_info:
			{
				info_va( f, l, info, ap );
			}
			break;
			case OutputLevel::out_warn:
			{
				warn_va( f, l, info, ap );
			}
			break;
			case OutputLevel::out_error:
			{
				error_va( f, l, info, ap );
			}
			break;
			case OutputLevel::out_panic:
			{
				panic_va( f, l, info, ap );
			}
			break;
			default: break;
		}
	}

	void Printer::log_out( OutputLevel level, const char *f, uint l, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		log_out_va( level, f, l, info, ap );
		va_end( ap );
	}

	void level_log_out( hsai::HsaiLogLevel level, const char *fn, uint ln, const char *info, ... )
	{
		va_list ap;
		va_start( ap, info );
		switch ( level )
		{
			case hsai::log_trace: k_printer.log_out_va( out_trace, fn, ln, info, ap ); break;
			case hsai::log_info : k_printer.log_out_va( out_info, fn, ln, info, ap ); break;
			case hsai::log_warn : k_printer.log_out_va( out_warn, fn, ln, info, ap ); break;
			case hsai::log_error: k_printer.log_out_va( out_error, fn, ln, info, ap ); break;
			case hsai::log_panic: k_printer.log_out_va( out_panic, fn, ln, info, ap ); break;
			default				: break;
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
} // namespace klib
