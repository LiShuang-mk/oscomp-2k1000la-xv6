//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "syscall/syscall_handler.hh"
#include "klib/common.hh"

namespace syscall
{
	SyscallHandler k_syscall_handler;

	void SyscallHandler::init()
	{
		for ( auto &func : _syscall_funcs )
		{
			func = [] () -> uint64
			{
				printf( "defualt syscall\n" );
				return 0;
			};
		}
	}

	uint64 SyscallHandler::invoke_syscaller( uint64 sys_num )
	{
		return _syscall_funcs[ sys_num ]();
	}
} // namespace syscall
