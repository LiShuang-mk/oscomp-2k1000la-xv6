//
// Created by Li Shuang ( pseudonym ) on 2024-04-27 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "klib/virtual_function.hh"
#include "klib/common.hh"

extern "C" {
	void _cxa_pure_virtual()
	{
		log_panic( "纯虚函数无法生成" );
	}
}