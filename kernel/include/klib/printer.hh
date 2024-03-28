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

class Console;

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
		Console *_console = nullptr;
		smp::Lock _lock;
		int _locking = 1;
		int _panicked = 0;
		static char _digits[];

	public:
		inline int is_panic() { return _panicked; }
		
		Printer() {};
		void init( Console *console, const char *name );
		void vprintf( const char *fmt, va_list ap );
		void printf( const char *fmt, ... );
		void printint( int xx, int base, int sign );
		void printptr( uint64 x );

		static void panic( const char *s );
	};

	extern Printer k_printer;
} // namespace kernellib
