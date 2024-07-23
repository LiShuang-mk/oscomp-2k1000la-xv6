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
	klib::k_printer.vprintf( fmt, ap );
	va_end( ap );
}

// void *memset( void *dst, int c, uint n )
// {
// 	char *cdst = ( char * ) dst;
// 	uint i;
// 	for ( i = 0; i < n; i++ )
// 	{
// 		cdst[ i ] = c;
// 	}
// 	return dst;
// }

// void *memcpy( void *dst, const void *src, int len )
// {
// 	if ( 0 == dst || 0 == src )
// 	{
// 		return 0;
// 	}

// 	void *ret = dst;

// 	if ( dst <= src || ( char * ) dst >= ( char * ) src + len )
// 	{
// //没有内存重叠，从低地址开始复制
// 		while ( len-- )
// 		{
// 			*( char * ) dst = *( char * ) src;
// 			dst = ( char * ) dst + 1;
// 			src = ( char * ) src + 1;
// 		}
// 	}
// 	else
// 	{
// 		//有内存重叠，从高地址开始复制
// 		src = ( char * ) src + len - 1;
// 		dst = ( char * ) dst + len - 1;
// 		while ( len-- )
// 		{
// 			*( char * ) dst = *( char * ) src;
// 			dst = ( char * ) dst - 1;
// 			src = ( char * ) src - 1;
// 		}
// 	}
// 	return ret;
// }

void bit_set( void *_data, uint _kbit )
{
	uint mv = _kbit / ( sizeof( uchar ) << 3 );
	uint rs = _kbit % ( sizeof( uchar ) << 3 );
	uchar *p = ( uchar * ) _data;
	p += mv;
	*p |= 1 << rs;
}

void bit_reset( void *_data, uint _kbit )
{
	uint mv = _kbit / ( sizeof( uchar ) << 3 );
	uint rs = _kbit % ( sizeof( uchar ) << 3 );
	uchar *p = ( uchar * ) _data;
	p += mv;
	*p &= ( uchar ) ~( 1 << rs );
}

bool bit_test( void *_data, uint _kbit )
{
	uint mv = _kbit / ( sizeof( uchar ) << 3 );
	uint rs = _kbit % ( sizeof( uchar ) << 3 );
	uchar *p = ( uchar * ) _data;
	p += mv;
	return ( ( *p ) & ( 1 << rs ) );
}

int compare( const char *s, const char *t, uint len )
{
	for ( uint i = 0; i < len; i++ )
		if ( *( s + i ) != *( t + i ) )
			return i;
	return 0;
}

int lowest_bit( uint64 x )
{
	if ( x == 0 )
		return -1;
	int pos = 0;
	for ( ; ( x & 1 ) == 0; x >>= 1, pos++ )
		;
	return pos;
}

int highest_bit( uint64 x )
{
	if ( x == 0 )
		return -1;
	int pos = 63;
	uint64 test = ( ~0UL ) << 63;
	for ( ; ( x & test ) == 0; x <<= 1, pos-- )
		;
	return pos;
}

namespace math
{
	uint64 power( uint64 x, uint64 y )
	{
		uint64 ans = 1;
		while ( y )
		{
			if ( y & 1UL )
				ans *= x;
			x *= x;
			y >>= 1;
		}
		return ans;
	} // namespace math
}

	// void panic( const char *s )
	// {
	// 	klib::k_printer.panic( s );
	// }

