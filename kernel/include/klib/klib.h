#include "common.hh"

extern "C"
{
#define STDIO 1
#define STRING 0

//string
#ifdef STRING
//void    *memset(void *s, int c, size_t n);
//void    *memcpy(void *dest, const void *src, size_t n);
void    *memmove(void *dest, const void *src, size_t n);
int      memcmp(const void *s1, const void *s2, size_t n);
size_t   strlen(const char *s);
char    *strcat    (char *dst, const char *src);
char    *strcpy    (char *dst, const char *src);
char    *strncpy   (char *dst, const char *src, size_t n);
int      strcmp    (const char *s1, const char *s2);
int      strncmp   (const char *s1, const char *s2, size_t n);
int      strncmpamb(const char *s1, const char *s2, size_t n);
char    *strchr    (const char *s, char c);
void     snstr     (char *dst, wchar const *src, int len);
//void    *memchr    (const void *src_void,int c,size_t length);
#endif

// stdlib.h
#ifdef STDLIB
void   srand     (unsigned int seed);
int    rand      (void);
void  *malloc    (size_t size);
void   free      (void *ptr);
int    abs       (int x);
int    atoi      (const char *nptr);
#endif

// stdio.h
#ifdef STDIO
//int    printf    (const char *format, ...);
int    sprintf   (char *str, const char *format, ...);
int    snprintf  (char *str, size_t size, const char *format, ...);
int    vsprintf  (char *str, const char *format, va_list ap);
int    vsnprintf (char *str, size_t size, const char *format, va_list ap);
int    putchar   (char c);
// void   _blockingputs(const char *);
// void   _nonblockingputs(const char *);
#endif

//libmath
float ceilf(float x);
//int __cxa_atexit(void (*func) (void *), void * arg, void * dso_handle);
}