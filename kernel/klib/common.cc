//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/common.hh"
#include "klib/printer.hh"

void printf( const char *fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	kernellib::k_printer.vprintf( fmt, ap );
	va_end( ap );
}

void *memset( void *dst, int c, uint n )
{
	char *cdst = ( char * ) dst;
	uint i;
	for ( i = 0; i < n; i++ )
	{
		cdst[ i ] = c;
	}
	return dst;
}

void panic( const char *s )
{
	kernellib::k_printer.panic( s );
}