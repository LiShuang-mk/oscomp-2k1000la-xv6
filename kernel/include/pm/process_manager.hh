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
#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace fs
{
	namespace fat
	{
		class Fat32DirEntry;
	}
}

namespace pm
{
	#define MAXARG 32
	class ProcessManager
	{
	private:
		smp::Lock _pid_lock;
		smp::Lock _wait_lock;
		int _cur_pid;
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
		mm::PageTable proc_pagetable(Pcb *p);
		void proc_freepagetable(mm::PageTable pt, uint64 sz);
		void freeproc(Pcb *p);
		void sched();
		int exec(eastl::string path, eastl::vector<eastl::string> args);
		int wait(uint64 addr);
		void exit(int state);
		int load_seg(mm::PageTable &pt, uint64 va, fs::fat::Fat32DirEntry *de, uint offset, uint size);
		void sleep(void *chan, smp::Lock *lock);
		void wakeup(void *chan);
		void vectortest();
		void stringtest();
		void maptest();
		void hashtest();
	};

	extern ProcessManager k_pm;
}