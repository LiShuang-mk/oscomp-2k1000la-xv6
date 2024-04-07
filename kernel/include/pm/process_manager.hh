//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 
#include "pm/process.hh"
#include "smp/lock.hh"

namespace pm
{
	class ProcessManager
	{
	private:
		smp::Lock _pid_lock;
		smp::Lock _wait_lock;

	public:
		ProcessManager() = default;
		void init( const char *pid_lock_name, const char *wait_lock_name );
		Pcb *get_cur_pcb();
		bool change_state( Pcb *p, ProcState state );
	};

	extern ProcessManager k_pm;
}