//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "./printer.hh"

void printf( const char *fmt, ... );
// void *memset( void *dst, int c, uint n );
// void *memcpy( void *dst, const void *src, int len );

/// @brief 给 _data 地址指向的数据的第 _kbit 位置位
/// @param _data 
/// @param _kbit 
void bit_set( void *_data, uint _kbit );

/// @brief 给 _data 地址指向的数据的第 _kbit 位复位
/// @param _data 
/// @param _kbit 
void bit_reset( void *_data, uint _kbit );

/// @brief 检测 _data 地址指向的数据的第 _kbit 位的值
/// @param _data 
/// @param _kbit 
/// @return 
bool bit_test( void *_data, uint _kbit );

/// @brief 比较字符串
/// @param s 字符串1
/// @param t 字符串2
/// @param len 字符串长度，保证两个字符串长度一样
/// @return 字符串相同返回0，否则返回不相同字符的偏移
int compare( const char *s, const char *t, uint len );

/// @brief 获取置位为1的最低位位置
/// @param x 
/// @return 如果x为0，则返回-1
int lowest_bit( uint64 x );

/// @brief 获取置位为1的最高位位置，使用负数会出现结果始终为63的问题
/// @param x 
/// @return 如果x为0，则返回-1
int highest_bit( uint64 x );

inline char unicode_to_ascii( uint16 uc )
{
	return ( char ) ( uc & 0xFFU );
}

namespace math
{
	/// @brief 求幂
	/// @param x 底数
	/// @param y 指数
	/// @return 
	uint64 power( uint64 x, uint64 y );
} // namespace math

#define assert(expr,detail,args...) ((expr)? (void)0 : klib::k_printer.assrt( __FILE__, __LINE__, #expr, detail,##args ))
#define log_panic(panic_info,args...) klib::k_printer.panic( __FILE__, __LINE__, panic_info,##args )

#ifdef OS_DEBUG

#define log_warn(warn__info,args...) klib::k_printer.warn( __FILE__, __LINE__, warn__info,##args )
#define log_info(info__info,args...) klib::k_printer.info( __FILE__, __LINE__, info__info,##args )
#define log_trace(trace_info, args...) klib::k_printer.trace( __FILE__, __LINE__, trace_info,##args )
#define log_error(error_info,args...) klib::k_printer.error( __FILE__, __LINE__, error_info,##args )

#else

#define log_error(error_info,args...) (void)(0)
#define log_warn(warn__info,args...) (void)(0)
#define log_info(info__info,args...) (void)(0)
#define log_trace(trace_info, args...) (void)(0)

#endif

#ifndef COLOR_PRINT
#define COLOR_PRINT

#define RED_COLOR_PINRT "\033[31m"
#define GREEN_COLOR_PRINT "\033[32m"
#define BLUE_COLOR_PRINT "\033[34m"
#define CYAN_COLOR_PINRT "\033[36m"
#define CLEAR_COLOR_PRINT "\033[0m"

#endif
