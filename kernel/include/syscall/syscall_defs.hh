//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

namespace syscall
{

#define _build_sysnum_enum_( name, number ) SYS_##name = number, 
	enum SyscallNumber
	{
		_build_sysnum_enum_( write, 64 )
	};
#undef _build_sysnum_enum_

} // namespace  syscall
