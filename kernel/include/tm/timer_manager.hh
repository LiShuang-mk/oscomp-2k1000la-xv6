//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "types.hh"

namespace tm
{
	// 2^20 * 4（4M）分频，则时间片 4M/1G ~ 4ms 
	constexpr uint div_fre = 0x1000000UL;

	class TimerManager
	{
	private:
		smp::Lock _lock;

	public:
		TimerManager() = default;
		void init( const char *lock_name );
	};

	extern TimerManager k_tm;
} // namespace tm