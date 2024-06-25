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
	void ( *p_hsai_logout )( const char * fn, uint ln, const char *info, ... ) = nullptr;
	
} // namespace hsai
