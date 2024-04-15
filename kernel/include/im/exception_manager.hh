//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace im
{
	class ExceptionManager
	{
	private:
		smp::Lock _lock;

	public:
		ExceptionManager() = default;
		void init( const char *lock_name );
		void kernel_trap();
		void machine_trap();
	};

	extern ExceptionManager k_em;
} // namespace im
