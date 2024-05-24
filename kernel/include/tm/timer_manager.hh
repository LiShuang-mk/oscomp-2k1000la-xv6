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

namespace tmm
{

	constexpr uint64 _1K_dec = 1000UL;
	constexpr uint64 _1M_dec = _1K_dec * _1K_dec;

	// 以下两个常量表达式应当是绑定在一起的

	// 参考测例说明的时钟频率是12.5MHz
	constexpr uint64 qemu_fre = 12 * _1M_dec + 500 * _1K_dec;
	// 由cycles计算出微秒的方法
	constexpr uint64 qemu_fre_cal_usec( uint64 cycles ) { return cycles * 2 / 25; }

	// 50K 分频，则时间片 50K/12.5MHz ~ 4ms 
	// constexpr uint div_fre = 0x80000000UL;
	constexpr uint div_fre = 50 * _1K_dec;
	constexpr uint ms_per_tick = div_fre * _1K_dec / qemu_fre;

	// 这个结构体来自Linux的定义 
	struct timeval
	{
		using time_t = uint64;
		using suseconds_t = uint64;

		time_t      tv_sec;     /* seconds */
		suseconds_t tv_usec;    /* microseconds */
	};

	class TimerManager
	{
	private:
		smp::Lock _lock;
		uint64 _ticks;

	public:
		TimerManager() = default;
		void init( const char *lock_name );
		int handle_clock_intr();

		timeval get_time_val();
	};

	extern TimerManager k_tm;
} // namespace tm
