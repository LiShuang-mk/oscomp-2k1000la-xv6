//
// Created by Li Shuang ( pseudonym ) on 2024-07-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include <process_interface.hh>

#include "pm/process_manager.hh"
#include "mm/memlayout.hh"

namespace hsai
{
	void * get_cur_proc()
	{
		return ( void * ) pm::k_pm.get_cur_pcb();
	}

	void * get_trap_frame_from_proc( void * proc )
	{
		return ( void * ) ( ( pm::Pcb * ) proc )->get_trapframe();
	}

	ulong get_trap_frame_vir_addr()
	{
		return mm::vml::vm_trap_frame;
	}

	uint get_pid( void * proc )
	{
		return ( ( pm::Pcb * ) proc )->get_pid();
	}

	bool proc_is_killed( void * proc )
	{
		return ( ( pm::Pcb * ) proc )->is_killed();
	}

	bool proc_is_running( void * proc )
	{
		return ( ( pm::Pcb * ) proc )->get_state() == pm::ProcState::running;
	}

	void sched_proc( void * proc )
	{
		pm::k_pm.sche_proc( ( pm::Pcb * ) proc );
	}

	ulong get_kstack_from_proc( void * proc )
	{
		return ( ( pm::Pcb * ) proc )->get_kstack();
	}

	ulong get_pgd_addr( void * proc )
	{
		return ( ( pm::Pcb * ) proc )->get_pagetable()->get_base();
	}

	void exit_proc( void * proc, int state )
	{
		pm::k_pm.exit_proc( ( pm::Pcb * ) proc, state );
	}

	PageTable * get_pt_from_proc( void * proc )
	{
		return ( PageTable * ) ( ( pm::Pcb * ) proc )->get_pagetable();
	}

} // namespace hsai
