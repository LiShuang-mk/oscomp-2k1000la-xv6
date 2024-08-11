//
// Created by Li Shuang ( pseudonym ) on 2024-07-30
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once
//#include <sys/resource.h>
namespace pm
{
	// following code is from Linux (include/uapi/asm-generic/resource.h)

	enum ResourceLimitId : uint
	{

	/*
	* Resource limit IDs
	*
	* ( Compatibility detail: there are architectures that have
	*   a different rlimit ID order in the 5-9 range and want
	*   to keep that order for binary compatibility. The reasons
	*   are historic and all new rlimits are identical across all
	*   arches. If an arch has such special order for some rlimits
	*   then it defines them prior including asm-generic/resource.h. )
	*/

		RLIMIT_CPU = 0,			/* CPU time in sec */
		RLIMIT_FSIZE = 1,		/* Maximum filesize */
		RLIMIT_DATA = 2,		/* max data size */
		RLIMIT_STACK = 3,		/* max stack size */
		RLIMIT_CORE = 4,		/* max core file size */

		RLIMIT_RSS = 5,			/* max resident set size */

		RLIMIT_NPROC = 6,		/* max number of processes */

		RLIMIT_NOFILE = 7,		/* max number of open files */

		RLIMIT_MEMLOCK = 8,		/* max locked-in-memory address space */

		RLIMIT_AS = 9,			/* address space limit */

		RLIMIT_LOCKS = 10,		/* maximum file locks held */
		RLIMIT_SIGPENDING = 11,	/* max number of pending signals */
		RLIMIT_MSGQUEUE = 12,	/* maximum bytes in POSIX mqueues */
		RLIMIT_NICE = 13,		/* max nice prio allowed to raise to
									0-39 for nice level 19 .. -20 */
		RLIMIT_RTPRIO = 14,		/* maximum realtime priority */
		RLIMIT_RTTIME = 15,		/* timeout for RT tasks in us */
		RLIM_NLIMITS = 16,

		/*
		* SuS says limits have to be unsigned.
		* Which makes a ton more sense anyway.
		*
		* Some architectures override this (for compatibility reasons):
		*/
		RLIM_INFINITY = ( ~0U ),

	}; // enum ResourceLimitId

	struct rlimit64
	{
		u64 rlim_cur;
		u64 rlim_max;
	};

} // namespace pm

