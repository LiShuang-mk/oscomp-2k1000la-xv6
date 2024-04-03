//
// Created by Li shuang ( pseudonym ) on 2024-04-02 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/page_table.hh"
#include "mm/memlayout.hh"
#include "klib/common.hh"

namespace mm
{
	PageTable k_pagetable;

	Pte mm::PageTable::walk( uint64 va, bool alloc )
	{
		if ( va >= vml::vm_end )
			log_panic( "va out of bounds" );
			return Pte();
	}
}