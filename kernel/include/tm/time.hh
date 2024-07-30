//
// Created by Li Shuang ( pseudonym ) on 2024-07-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace tmm
{
	// following code is from Linux (include/uapi/linux/time.h)

	struct timespec
	{
		long	tv_sec;		/* seconds */
		long	tv_nsec;	/* nanoseconds */
	};

	enum SystemClockId : uint
	{
	/*
	* The IDs of the various system clocks (for POSIX.1b interval timers):
	*/
		CLOCK_REALTIME = 0,
		CLOCK_MONOTONIC = 1,
		CLOCK_PROCESS_CPUTIME_ID = 2,
		CLOCK_THREAD_CPUTIME_ID = 3,
		CLOCK_MONOTONIC_RAW = 4,
		CLOCK_REALTIME_COARSE = 5,
		CLOCK_MONOTONIC_COARSE = 6,
		CLOCK_BOOTTIME = 7,
		CLOCK_REALTIME_ALARM = 8,
		CLOCK_BOOTTIME_ALARM = 9,
	   /*
		* The driver implementing this got removed. The clock ID is kept as a
		* place holder. Do not reuse!
		*/
		CLOCK_SGI_CYCLE = 10,
		CLOCK_TAI = 11,

		MAX_CLOCKS = 16
	};

} // namespace tmm
