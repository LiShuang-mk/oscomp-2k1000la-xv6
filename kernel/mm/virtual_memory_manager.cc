//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/loongarch.hh"
#include "hal/cpu.hh"
#include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"
#include "mm/page.hh"
#include "mm/page_table.hh"
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
		k_pmm.clear_page( ( void* ) addr );
		k_pagetable.set_base( addr );

		for ( pm::Pcb &pcb : pm::k_proc_pool )
		{
			pcb.map_kstack( mm::k_pagetable );
		}


	}

	bool VirtualMemoryManager::map_pages( PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags )
	{
		uint64 a, last;
		Pte pte;

		if ( size == 0 )
			log_panic( "mappages: size" );

		a = page_round_down( va );
		last = page_round_down( va + size - 1 );
		for ( ;;)
		{
			pte = pt.walk( a, /*alloc*/ true );
			if ( pte.is_null() )
			{
				log__warn( "walk failed" );
				return false;
			}
			if ( pte.is_valid() )
				log_panic( "mappages: remap" );
			// pte = PA2PTE( pa ) | perm | loongarch::PteEnum::valid_m;
			pte.set_data( page_round_down( pa ) | flags | loongarch::PteEnum::valid_m );
			if ( a == last )
				break;
			a += pg_size;
			pa += pg_size;
		}
		return true;
	}
}