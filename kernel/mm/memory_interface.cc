//
// Created by Li Shuang ( pseudonym ) on 2024-07-19
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include <memory_interface.hh>

#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"

namespace hsai
{
	void *alloc_pages( uint cnt )
	{
		void *p = mm::k_pmm.alloc_pages( cnt );
		mm::k_pmm.clear_pages( p, cnt );
		return p;
	}

	int free_pages( void *ptr ) { return mm::k_pmm.free_pages( ptr ); }

	int copy_to_user( VirtualPageTable *pt, uint64 va, const void *p, uint64 len )
	{
		return mm::k_vmm.copyout( *( (mm::PageTable *) pt ), va, p, len );
	}
	int copy_from_user( VirtualPageTable *pt, void *dst, uint64 src_va, uint64 len )
	{
		return mm::k_vmm.copy_in( *( (mm::PageTable *) pt ), dst, src_va, len );
	}

} // namespace hsai
