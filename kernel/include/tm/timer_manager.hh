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

	// 参考测例说明的时钟频率是12.5MHz, 但是测试发现比这个值要小一些，大约是四分之一
	// constexpr uint64 qemu_fre = 12 * _1M_dec + 500 * _1K_dec;
	constexpr uint64 qemu_fre = 3 * _1M_dec + 125 * _1K_dec;
	// 由cycles计算出微秒的方法
	constexpr uint64 qemu_fre_cal_usec( uint64 cycles ) { return cycles * 8 / 25; }
	// 由微秒计算出cycles的方法
	constexpr uint64 qemu_fre_cal_cycles( uint64 usec ) { return usec * 25 / 8; }

	// 100K 分频，则时间片 100K/3.125MHz ~ 32ms 
	// constexpr uint div_fre = 0x80000000UL;
	constexpr uint div_fre = ( 1000 * _1K_dec ) >> 2;				// 低两位由硬件补齐
	constexpr uint ms_per_tick = div_fre * _1K_dec / qemu_fre;

	// 这个结构体来自Linux的定义 
	struct timeval
	{
		using time_t = uint64;
		using suseconds_t = uint64;

		time_t      tv_sec;     /* seconds */
		suseconds_t tv_usec;    /* microseconds */
	};

	// 这个结构体来自Linux的定义 
	struct tms
	{
		uint64 tms_utime;
		uint64 tms_stime;
		uint64 tms_cutime;
		uint64 tms_cstime;
	};

	class TimerManager
	{
	private:
		smp::Lock _lock;
		uint64 _ticks;
		uint64 _tcfg_data;

	public:
		TimerManager() = default;
		void init( const char *lock_name );
		int handle_clock_intr();

		timeval get_time_val();

		int sleep_n_ticks( int n );

		int sleep_from_tv( timeval tv );

		void open_ti_intr();
		void close_ti_intr();

	public:
		uint64 get_ticks() { return _ticks; };
	};

	extern TimerManager k_tm;
} // namespace tm
