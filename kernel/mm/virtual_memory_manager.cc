//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/loongarch.hh"
#include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"
#include "mm/page.hh"
#include "mm/memlayout.hh"
#include "pm/process.hh"

namespace mm
{
	VirtualMemoryManager k_vmm;

	uint64 VirtualMemoryManager::kstack_vm_from_gid( uint gid )
	{
		if ( gid >= pm::num_process )
			log_panic( "vmm: invalid gid" );
		return ( vml::vm_trap_frame - ( ( ( gid + 1 ) << 1 ) << pg_size_shift ) );
	}

	void VirtualMemoryManager::init()
	{
		uint64 addr = ( uint64 ) k_pmm.alloc_page();
		if ( addr == 0 )
			log_panic( "vmm init" );
		k_pagetable.set_base( addr );
		memset( ( void* ) addr, 0, pg_size );

		// to do

	}

	bool VirtualMemoryManager::map_pages( const PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags )
	{
		// uint64 a, last;
		// pte_t *pte;

		// if ( size == 0 )
		// 	log_panic( "mappages: size" );

		// a = page_round_up( va );
		// last = page_round_down( va + size - 1 );
		// for ( ;;)
		// {
		// 	if ( ( pte = walk( pt, a, 1 ) ) == 0 )
		// 		return -1;
		// 	if ( *pte & loongarch:: )
		// 		panic( "mappages: remap" );
		// 	*pte = PA2PTE( pa ) | perm | loongarch::PteEnum::valid_m;
		// 	if ( a == last )
		// 		break;
		// 	a += PGSIZE;
		// 	pa += PGSIZE;
		// }
		return 0;
	}

	pte_t *VirtualMemoryManager::walk( const PageTable &pt, uint64 va, bool alloc )
	{
		return nullptr;
	}

	bool VirtualMemoryManager::_walk_to_next_level( PageTable &pt, pte_t *pte, bool alloc )
	{
		return 0;
	}
}