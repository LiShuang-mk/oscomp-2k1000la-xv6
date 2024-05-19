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
		int _cur_pid;
		Pcb * _init_proc;		// user init proc

	public:
		ProcessManager() = default;
		void init( const char *pid_lock_name, const char *wait_lock_name );
		Pcb *get_cur_pcb();
		bool change_state( Pcb *p, ProcState state );
		void alloc_pid(Pcb *p);
		Pcb *alloc_proc();
		void set_slot( Pcb *p, int slot );
		void set_priority( Pcb *p, int priority );
		void set_shm(Pcb *p);
		void set_vma(Pcb *p);
		int set_trapframe(Pcb *p);
		void freeproc(Pcb *p);
		void vectortest();
		void stringtest();
		void maptest();
		void hashtest();

		void exit( int );

		void user_init();

	private:
		void _proc_create_vm( Pcb * p );
	};

	extern ProcessManager k_pm;
}