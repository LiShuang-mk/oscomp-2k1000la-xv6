//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "klib/function.hh"

namespace syscall
{
	constexpr uint max_syscall_funcs_num = 2048;

	class SyscallHandler
	{
	private:
		std::function<uint64( void )> _syscall_funcs[ max_syscall_funcs_num ];


	public:
		SyscallHandler() {};
		void init();
		uint64 invoke_syscaller( uint64 sys_num );

		
	};

	extern SyscallHandler k_syscall_handler;

} // namespace syscall
