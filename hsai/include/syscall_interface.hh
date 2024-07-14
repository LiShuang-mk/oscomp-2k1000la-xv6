//
// Created by Li Shuang ( pseudonym ) on 2024-07-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include <kernel/types.hh>

namespace hsai
{
	extern __hsai_kernel ulong kernel_syscall( ulong sys_num );
	extern __hsai_kernel ulong get_syscall_max_num();

} // namespace hsai
