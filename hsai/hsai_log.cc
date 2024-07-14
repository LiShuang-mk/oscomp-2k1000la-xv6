//
// Created by Li Shuang ( pseudonym ) on 2024-06-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hsai_log.hh"

namespace hsai
{
	void ( *p_hsai_logout )( HsaiLogLevel log_level, const char * fn, uint ln, const char *info, ... ) = nullptr;
	void ( *p_hsai_assert )( const char *f, uint l, const char *expr, const char *detail, ... ) = nullptr;
	void ( *p_hsai_printf )( const char *fmt, ... ) = nullptr;

} // namespace hsai
