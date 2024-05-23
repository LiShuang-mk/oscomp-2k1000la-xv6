#ifndef KLIB_H__
#define KLIB_H__


#include "common.hh"

#if defined(__cplusplus)
// #include "klib.hh"
#endif


extern "C" {
	void  *memset( void *s, int c, size_t n ) noexcept( true );
	void  *memcpy( void *dst, const void *src, size_t n ) noexcept( true );
	void  *memmove( void *dst, const void *src, size_t n ) noexcept( true );
	int    memcmp( const void *s1, const void *s2, size_t n ) noexcept( true );
	size_t strlen( const char *s ) noexcept( true );
	char  *strcat( char *dst, const char *src ) noexcept( true );
	char  *strcpy( char *dst, const char *src ) noexcept( true );
	char  *strncpy( char *dst, const char *src, size_t n ) noexcept( true );
	int    strcmp( const char *s1, const char *s2 ) noexcept( true );
	int    strncmp( const char *s1, const char *s2, size_t n ) noexcept( true );
}


extern "C++" {
#define STDIO 1
#define STRING 0

// string.h
#ifdef STRING
	int    strncmpamb( const char *s1, const char *s2, size_t n );
	char  *strchr( const char *s, char c );
	void   snstr( char *dst, wchar const *src, int len );
	const void * memchr( const void *src_void, int c, size_t length ) noexcept( true );
#endif

// stdlib.h
#ifdef STDLIB
	void   srand( unsigned int seed );
	int    rand( void );
	void  *malloc( size_t size );
	void   free( void *ptr );
	int    abs( int x );
	int    atoi( const char *nptr );
#endif
// stdio.h
#ifdef STDIO
//#pragma message("klib.h: STDIO")
//int    printf    (const char *format, ...);
	int    sprintf( char *str, const char *format, ... );
	int    snprintf( char *str, size_t size, const char *format, ... );
	int    vsprintf( char *str, const char *format, va_list ap );
	int    vsnprintf( char *str, size_t size, const char *format, va_list ap );
	int    putchar( char c );
	// void   _blockingputs(const char *);
	// void   _nonblockingputs(const char *);

	//libm
	float ceilf( float x );

}
#endif

#endif