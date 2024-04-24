//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/function.hh"
#include "klib/common.hh"

namespace std
{
	void __throw_bad_function_call()
	{
		log_panic( "! >>> bad function call" );
	}
} // namespace std
