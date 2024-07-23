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

	class Printer
	{
	private:
		enum out_type
		{
			console,
			file,
			device,
		} _type;
		dev::Console *_console = nullptr;
		hsai::SpinLock _lock;
		int _locking = 1;
		int _panicked = 0;
		static int _trace_flag;
		static char _digits[];

	public:
		inline int is_panic() { return _panicked; }

		Printer() {};
		void init( dev::Console *console, const char *name );
		void vprintf( const char *fmt, va_list ap );
		void printf( const char *fmt, ... );
		void printint( int xx, int base, int sign );
		void printbyte( uint8 x );
		void printptr( uint64 x );

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
	};

	extern Printer k_printer;

	void level_log_out( hsai::HsaiLogLevel level, const char * fn, uint ln, const char * info, ... );
	void assert_log_out( const char *f, uint l, const char *expr, const char *detail, ... );
	void printf_log_out( const char *fmt, ... );

} // namespace klib
