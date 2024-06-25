//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace hsai
{
	extern void ( *p_hsai_logout )( const char * fn, uint ln, const char *info, ... );
} // namespace hsai

#define hsai_trace(trace_info,args...) (hsai::p_hsai_logout)?(hsai::p_hsai_logout(__FILE__,__LINE__,trace_info,##args)):((void)0)
#define hsai_info(info__info,args...) (hsai::p_hsai_logout)?(hsai::p_hsai_logout(__FILE__,__LINE__,info__info,##args)):((void)0)
#define hsai_warn(warn__info,args...) (hsai::p_hsai_logout)?(hsai::p_hsai_logout(__FILE__,__LINE__,warn__info,##args)):((void)0)
#define hsai_error(error_info,args...) (hsai::p_hsai_logout)?(hsai::p_hsai_logout(__FILE__,__LINE__,error_info,##args)):((void)0)
#define hsai_panic(panic_info,args...) (hsai::p_hsai_logout)?(hsai::p_hsai_logout(__FILE__,__LINE__,panic_info,##args)):((void)0)
// #define hsai_assert(expr,detail,args...) ((expr)? (void)0 : kernellib::k_printer.assrt( __FILE__, __LINE__, #expr, detail,##args ))
