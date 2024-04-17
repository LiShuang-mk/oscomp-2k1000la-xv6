//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "klib/printer.hh"

void printf( const char *fmt, ... );
void ktrace( const char *f, uint l, const char *info, ... );
void *memset( void *dst, int c, uint n );

/// @brief 比较字符串
/// @param s 字符串1
/// @param t 字符串2
/// @param len 字符串长度，保证两个字符串长度一样
/// @return 字符串相同返回0，否则返回不相同字符的偏移
int compare( const char *s, const char *t, uint len );
// void panic( const char *s );

#define log_panic(panic_info) kernellib::k_printer.panic( __FILE__, __LINE__, panic_info )
#define log_error(error_info) kernellib::k_printer.error( __FILE__, __LINE__, error_info )
#define log__warn(warn__info) kernellib::k_printer.warn( __FILE__, __LINE__, warn__info )
#define log__info(info__info) kernellib::k_printer.info( __FILE__, __LINE__, info__info )

#define log_trace(trace_info, args...) ktrace( __FILE__, __LINE__, trace_info, args )

#define assert(expr) ((expr)? (void)0 : kernellib::k_printer.assrt( __FILE__, __LINE__, #expr ))