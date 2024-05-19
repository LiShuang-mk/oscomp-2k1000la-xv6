//
// Created by Li Shuang ( pseudonym ) on 2024-05-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "syscall/syscall.h"

uint64_t( *syscall_funcs[ _MAX_SYSCALL_NUM_ ] )( void ) =
{
	
};