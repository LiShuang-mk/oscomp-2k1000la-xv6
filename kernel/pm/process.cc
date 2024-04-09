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
#include "mm/page_table.hh"
#include "mm/page.hh"
#include "klib/common.hh"
#include "pm/sharemem.hh"
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
		if ( _kstack == 0 )
			log_panic( "pcb was not init" );
		char *pa = ( char * ) mm::k_pmm.alloc_page();
		if ( pa == 0 )
			log_panic( "pcb map kstack: no memory" );
		mm::k_pmm.clear_page( ( void* ) pa );
		flag_t pg_flag = loongarch::PteEnum::nx_m
			| loongarch::PteEnum::presence_m
			| loongarch::PteEnum::writable_m
			| ( loongarch::MatEnum::mat_cc << loongarch::PteEnum::mat_s )
			| loongarch::PteEnum::dirty_m;
		if ( !mm::k_vmm.map_pages( pt, _kstack, mm::pg_size, ( uint64 ) pa, pg_flag ) )
			log_panic( "kernel vm map failed" );

		printf( "map kstack : %p => %p (gid=%d)\n", _kstack, pt.walk( _kstack, 0 ).pa(), _gid );
	}

	ProcState Pcb::get_state()
	{
		_lock.acquire();
		ProcState state = _state;
		_lock.release();
		return state;
	}

	int Pcb::get_priority()
	{
		_lock.acquire();
		int priority = _priority;
		_lock.release();
		return priority;
	}
}
