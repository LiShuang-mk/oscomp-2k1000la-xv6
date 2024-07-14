//
// Created by Li Shuang ( pseudonym ) on 2024-07-10 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hsai_global.hh"
#include "virtual_cpu.hh"

namespace hsai
{
	VirtualCpu * k_cpus[ NUMCPU ] = { nullptr };

	VirtualCpu * get_cpu()
	{
		u32 cpuid = k_cpus[ 0 ]->get_cpu_id();
		return k_cpus[ cpuid ];
	}

	VirtualMemory * k_mem = nullptr;

	VirtualInterruptManager * k_im = nullptr;

} // namespace hsai
