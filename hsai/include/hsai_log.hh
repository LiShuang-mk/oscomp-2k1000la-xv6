//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "kernel/types.hh"

namespace hsai
{
	enum HsaiLogLevel
	{
		log_trace,
		log_info,
		log_warn,
		log_error,
		log_panic,
	};

	extern void ( *p_hsai_logout )( HsaiLogLevel log_level, const char * fn, uint ln, const char *info, ... );
	extern void ( *p_hsai_assert )( const char *f, uint l, const char *expr, const char *detail, ... );
	extern void ( *p_hsai_printf )( const char *fmt, ... );
} // namespace hsai

#define hsai_trace(trace_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_trace,__FILE__,__LINE__,trace_info,##args)):((void)0); \
	}while(0)

#define hsai_info(info__info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_info,__FILE__,__LINE__,info__info,##args)):((void)0); \
	}while(0)

#define hsai_warn(warn__info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_warn,__FILE__,__LINE__,warn__info,##args)):((void)0); \
	}while(0)

#define hsai_error(error_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_error,__FILE__,__LINE__,error_info,##args)):((void)0); \
	}while(0)

#define hsai_panic(panic_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_panic,__FILE__,__LINE__,panic_info,##args)):((void)0); \
		while(1); \
	}while(0)

#define hsai_assert(expr,detail,args...) \
	do{ \
		(hsai::p_hsai_assert)?( \
			(expr)? \
				( void ) 0 : \
				(hsai::p_hsai_assert(__FILE__,__LINE__,#expr,detail,##args)) ) : \
			(void)0; \
	}while(0)

#define hsai_printf(fmt,args...) \
	do{ \
		(hsai::p_hsai_printf)?(hsai::p_hsai_printf(fmt,##args)):((void)0); \
	}while(0)

#ifndef COLOR_PRINT
#define COLOR_PRINT

#define RED_COLOR_PINRT "\033[31m"
#define GREEN_COLOR_PRINT "\033[32m"
#define YELLOW_COLOR_PRINT "\033[33m"
#define BLUE_COLOR_PRINT "\033[34m"
#define MAGANTA_COLOR_PRINT "\033[35m"
#define CYAN_COLOR_PINRT "\033[36m"
#define CLEAR_COLOR_PRINT "\033[0m"

#define BRIGHT_RED_COLOR_PINRT "\033[91m"
#define BRIGHT_GREEN_COLOR_PRINT "\033[92m"
#define BRIGHT_YELLOW_COLOR_PRINT "\033[93m"
#define BRIGHT_BLUE_COLOR_PRINT "\033[94m"
#define BRIGHT_MAGANTA_COLOR_PRINT "\033[95m"
#define BRIGHT_CYAN_COLOR_PINRT "\033[96m"

#endif