//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "tm/time.hh"

#include <smp/spin_lock.hh>

namespace tmm
{
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
		hsai::SpinLock _lock;
		uint64 _ticks;
		// uint64 _tcfg_data;

	public:
		TimerManager() = default;
		void init( const char *lock_name );
		int handle_clock_intr();
		void tick_increase() { _lock.acquire(); _ticks++; _lock.release(); }

		timeval get_time_val();

		int sleep_n_ticks( int n );

		int sleep_from_tv( timeval tv );

		int clock_gettime( SystemClockId clockid, timespec * tp );

		// void open_ti_intr();
		// void close_ti_intr();

	public:
		uint64 get_ticks() { return _ticks; };
	};

	extern TimerManager k_tm;
} // namespace tm
