//
// Created by Li Shuang ( pseudonym ) on 2024-07-10 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mem/virtual_memory.hh"
#include "hsai_global.hh"
#include "hsai_log.hh"

namespace hsai
{
	int VirtualMemory::register_memory( VirtualMemory * mem )
	{
		if ( k_mem != nullptr )
			hsai_panic( "repeat register memory" );
		k_mem = mem;
		return 0;
	}

} // namespace hsai
