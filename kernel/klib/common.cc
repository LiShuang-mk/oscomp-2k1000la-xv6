//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/common.hh"

void printf( const char *fmt, ... )
{
	va_list ap;
	va_start( ap, fmt );
	kernellib::k_printer.vprintf( fmt, ap );
	va_end( ap );
}

void ktrace( const char *f, uint l, const char *info, ... )
{
	va_list ap;
	va_start( ap, info );
	kernellib::k_printer.trace( f, l, info, ap );
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

int compare( const char *s, const char *t, uint len )
{
	for (uint i = 0; i < len; i++)
		if ( *( s + i ) != *( t + i ) )
			return i;
	return 0;
}

// void panic( const char *s )
// {
// 	kernellib::k_printer.panic( s );
// }

