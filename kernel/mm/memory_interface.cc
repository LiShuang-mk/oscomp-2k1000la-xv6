//
// Created by Li Shuang ( pseudonym ) on 2024-07-19 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include <memory_interface.hh>

#include "mm/physical_memory_manager.hh"

namespace hsai
{
	void * alloc_pages( uint cnt )
	{
		return mm::k_pmm.alloc_pages( cnt );
	}

	int free_pages( void * ptr )
	{
		return mm::k_pmm.free_pages( ptr );
	}

} // namespace hsai
