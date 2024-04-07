//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/process_manager.hh"
#include "pm/process.hh"
#include "hal/cpu.hh"
namespace pm
{
	ProcessManager k_pm;

	void ProcessManager::init( const char *pid_lock_name, const char *wait_lock_name )
	{
		_pid_lock.init( pid_lock_name );
		_wait_lock.init( wait_lock_name );
		for ( uint i = 0; i < num_process; ++i )
		{
			Pcb &p = k_proc_pool[ i ];
			p.init( "pcb", i );
		}
	}

	Pcb *ProcessManager::get_cur_pcb()
	{
		loongarch::Cpu::push_intr_off();
		loongarch::Cpu *c_cpu = loongarch::Cpu::get_cpu();
		pm::Pcb *pcb = c_cpu->get_cur_proc();
		loongarch::Cpu::pop_intr_off();
		return pcb;
	}

	bool ProcessManager::change_state( Pcb *p, ProcState state )
	{
		_pid_lock.acquire();
		p->_lock.acquire();
		if ( p->_state == ProcState::unused )
		{
			p->_lock.release();
			_pid_lock.release();
			return false;
		}
		p->_state = state;
		p->_lock.release();
		_pid_lock.release();
		return true;
	}
}