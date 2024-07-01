//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "hsai_types.hh"

namespace hsai
{
	enum HsaiLogLevel
	{
		log_trace,
		log_info,
		log_warn,
		log_error,
		log_panic
	};

	extern void ( *p_hsai_logout )( HsaiLogLevel log_level, const char * fn, uint ln, const char *info, ... );
} // namespace hsai

#define hsai_trace(trace_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_trace,__FILE__,__LINE__,panic_info,##args)):((void)0); \
	}while(0)

#define hsai_info(info__info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_info,__FILE__,__LINE__,panic_info,##args)):((void)0); \
	}while(0)

#define hsai_warn(warn__info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_warn,__FILE__,__LINE__,panic_info,##args)):((void)0); \
	}while(0)

#define hsai_error(error_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_error,__FILE__,__LINE__,panic_info,##args)):((void)0); \
	}while(0)

#define hsai_panic(panic_info,args...) \
	do{ \
		(hsai::p_hsai_logout)?(hsai::p_hsai_logout(hsai::log_panic,__FILE__,__LINE__,panic_info,##args)):((void)0); \
		while(1); \
	}while(0)

// #define hsai_assert(expr,detail,args...) ((expr)? (void)0 : kernellib::k_printer.assrt( __FILE__, __LINE__, #expr, detail,##args ))
