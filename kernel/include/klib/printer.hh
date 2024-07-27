//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <stdarg.h>

#include <smp/spin_lock.hh>
#include <hsai_log.hh>

namespace dev
{
	class Console;
} // namespace dev

namespace hsai
{
	class StreamDevice;
} // namespace hsai


namespace klib
{
	enum OutputLevel
	{
		out_trace,
		out_info,
		out_warn,
		out_error,
		out_panic,
	};

	constexpr long printer_buffer_size = 4096;
	class PrinterBuffer
	{
	private:
		char * _buf = nullptr;
		int _buf_len = 0;
		char * _write_header = _buf;
		char * _end_ptr = _buf;
	public:
		PrinterBuffer() = default;
		PrinterBuffer( char *buf_ptr, int buf_len ) : _buf( buf_ptr ), _buf_len( buf_len ) { _end_ptr += _buf_len; };
		void clear() { _write_header = _buf; }
		int size() { return _buf_len; }
		void put( char c ) { if ( _write_header < _end_ptr ) { *_write_header = c; _write_header++; } }
		void go_back_one() { if ( _write_header > _buf ) _write_header--; }
		int content_length() { return _write_header - _buf; }
		const char * get_content() { return ( const char * ) _buf; }
	};

	class Printer
	{
	private:
		enum out_type
		{
			console,
			file,
			device,
		} _type;
		struct print_flag
		{
			u8 zero_pad : 1;
			u8 sign : 1;
			u8 plus : 1;
			u8 space : 1;
			u8 left : 1;
			u8 prefix : 1;
			u8 upper : 1;
			u8 seperate : 1;
		}__attribute__( ( __packed__ ) );

		hsai::StreamDevice *_console = nullptr;
		hsai::SpinLock _lock;
		int _locking = 1;
		int _panicked = 0;

		char _internal_buffer[ printer_buffer_size ];
		PrinterBuffer _buffer;

		static int _trace_flag;
		static char _lower_digits[];
		static char _upper_digits[];

	public:
		inline int is_panic() { return _panicked; }

		Printer() {};
		void init( const char *name );
		void vprintf( const char *fmt, va_list ap );
		void printf( const char *fmt, ... );
		void printint( int xx, int base, int sign );
		void printbyte( uint8 x );
		void printptr( uint64 x );

		void print_number( PrinterBuffer &buf, ulong num, int base, print_flag flag, char qualfier, int width );
		void vbprintf( PrinterBuffer &buf, const char *fmt, va_list args );

		static void panic( const char *f, uint l, const char *info, ... );
		static void error( const char *f, uint l, const char *info, ... );
		static void warn( const char *f, uint l, const char *info, ... );
		static void info( const char *f, uint l, const char *info, ... );
		static void trace( const char *f, uint l, const char *info, ... );
		static void assrt( const char *f, uint l, const char *expr, const char *detail, ... );
		static void assrt_va( const char *f, uint l, const char *expr, const char *detail, va_list ap );

		static void log_out( OutputLevel level, const char * f, uint l, const char * info, ... );
		static void log_out_va( OutputLevel level, const char * f, uint l, const char * info, va_list ap );
		static void log_output_info( const char * f, uint l, const char * info, va_list ap );

	private:
		int _divide( ulong &n, int base )
		{
			int res = ( int ) ( n % base );
			n = n / base;
			return res;
		}
		bool _is_number( char c ) { return ( unsigned ) ( c - '0' ) < 10; }
		int _to_number( char c ) { return c - '0'; }
	};

	extern Printer k_printer;

	void level_log_out( hsai::HsaiLogLevel level, const char * fn, uint ln, const char * info, ... );
	void assert_log_out( const char *f, uint l, const char *expr, const char *detail, ... );
	void printf_log_out( const char *fmt, ... );

} // namespace klib
