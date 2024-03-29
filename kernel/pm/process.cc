//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/process.hh"
#include "mm/virtual_memory_manager.hh"

Pcb k_proc_pool[ num_process ];

void Pcb::init( const char *lock_name, uint gid )
{
	_lock.init( lock_name );
	_state = ProcState::unused;
	_gid = gid;
	_kstack = VirtualMemoryManager::kstack_vm_from_gid( _gid );
}
