//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/process.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"

namespace pm
{
	Pcb k_proc_pool[ num_process ];

	void Pcb::init( const char *lock_name, uint gid )
	{
		_lock.init( lock_name );
		_state = ProcState::unused;
		_gid = gid;
		_kstack = mm::VirtualMemoryManager::kstack_vm_from_gid( _gid );
	}

	void Pcb::map_kstack( mm::PageTable &pt )
	{
		char *pa = ( char * ) mm::k_pmm.alloc_page();
		if ( pa == 0 )
			log_panic( "pcb map kstack: no memory" );

	}
}
