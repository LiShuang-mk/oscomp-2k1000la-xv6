//
// Created by Li shuang ( pseudonym ) on 2024-04-04 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace mm
{
	class TlbManager
	{
	private:
		smp::Lock _lock;

	public:
		TlbManager() = default;

		void init( const char *lock_name );

		inline void invalid_all_tlb()
		{
			asm volatile( "invtlb  0x0,$zero,$zero" );
		}
	};

	extern TlbManager k_tlbm;
} // namespace mm
