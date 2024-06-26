//
// Created by Li shuang ( pseudonym ) on 2024-03-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <stdarg.h>

#include "smp/lock.hh"
namespace dev
{
	class Console;
} // namespace dev

namespace kernellib
{
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
		smp::Lock _lock;
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
	};

	extern Printer k_printer;
} // namespace kernellib
