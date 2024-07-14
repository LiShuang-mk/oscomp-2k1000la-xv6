//
// Created by Li Shuang ( pseudonym ) on 2024-07-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include <syscall_interface.hh>
#include <syscall/syscall_handler.hh>

namespace hsai
{
	ulong kernel_syscall( ulong sys_num )
	{
		return syscall::k_syscall_handler.invoke_syscaller( sys_num );
	}

	ulong get_syscall_max_num()
	{
		return syscall::max_syscall_funcs_num;
	}

} // namespace hsai
