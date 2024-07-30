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

#include <process_interface.hh>
namespace pm
{
	Pcb k_proc_pool[ num_process ];

	Pcb::Pcb()
	{

	}

	void Pcb::init( const char *lock_name, uint gid )
	{
		_lock.init( lock_name );
		_state = ProcState::unused;
		_gid = gid;
		_kstack = mm::VirtualMemoryManager::kstack_vm_from_gid( _gid );
		new ( &_pt ) mm::PageTable();
		for ( auto &of : _ofile )
			of = nullptr;
		_context = hsai::get_context_address( _gid );
		if ( _context == nullptr )
			log_panic( "process %d : context set fail", _gid );

		// 配置资源限制

		_rlim_vec[ ResourceLimitId::RLIMIT_STACK ].rlim_cur = default_proc_ustack_pages * hsai::page_size;
		_rlim_vec[ ResourceLimitId::RLIMIT_STACK ].rlim_max = default_proc_ustack_pages * hsai::page_size;

		for (int i = 0; i < SIGRTMAX; ++i) {
            _sigactions[i] = nullptr;
        } // init sigactions

	}

	void Pcb::map_kstack( mm::PageTable &pt )
	{
		if ( _kstack == 0 )
			log_panic( "pcb was not init" );

		char *pa;

		// map the first page
		pa = ( char * ) mm::k_pmm.alloc_pages( default_proc_kstack_pages );
		if ( pa == 0 )
			log_panic( "pcb map kstack: no memory" );
		mm::k_pmm.clear_page( ( void* ) pa );
		if ( !mm::k_vmm.map_data_pages( pt, _kstack, hsai::page_size * default_proc_kstack_pages, ( uint64 ) pa, false ) )
			log_panic( "kernel vm map failed" );

		// // map the second page
		// pa = ( char * ) mm::k_pmm.alloc_page();
		// if ( pa == 0 )
		// 	log_panic( "pcb map kstack: no memory" );
		// mm::k_pmm.clear_page( ( void* ) pa );
		// if ( !mm::k_vmm.map_pages( pt, _kstack + mm::pg_size, mm::pg_size, ( uint64 ) pa, pg_flag ) )
		// 	log_panic( "kernel vm map failed" );

		printf( "map kstack : %p => %p (gid=%d)\n", _kstack, pt.walk( _kstack, 0 ).to_pa(), _gid );
		// uint64 tmp = *( uint64* ) _kstack;
		// printf( "read kstack %p : %d", _kstack, tmp );
	}

	int Pcb::get_priority()
	{
		_lock.acquire();
		int priority = _priority;
		_lock.release();
		return priority;
	}
}
