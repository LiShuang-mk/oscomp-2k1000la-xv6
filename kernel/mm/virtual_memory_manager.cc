//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"
#include "mm/page.hh"

void VirtualMemoryManager::init()
{
	PageTable k_pagetable;
	uint64 addr = ( uint64 ) k_pmm.alloc_page();
	if ( addr == 0 )
		panic( "vmm init" );
	k_pagetable.set_base( addr );
	memset( ( void* ) addr, 0, pg_size );

	// to do
}