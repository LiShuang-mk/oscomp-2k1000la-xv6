//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "mm/page_table.hh"
#include "pm/context.hh"
#include "pm/sharemem.hh"

namespace fs
{
	class Dentry;
}
namespace pm
{
	struct TrapFrame;

	constexpr uint num_process = 32;
	constexpr int default_proc_prio = 10;
	constexpr int lowest_proc_prio = 19;
	constexpr int highest_proc_prio = 0;

	enum ProcState
	{
		unused,
		used,
		sleeping,
		runnable,
		running,
		zombie
	};

	class ProcessManager;
	class ShmManager;
	class Scheduler;

	class Pcb
	{
		friend ProcessManager;
		friend ShmManager;
		friend Scheduler;
	private:
		smp::Lock _lock;
		int _gid = num_process;					// global ID in pool 

		fs::Dentry *_cwd;				// current working directory
		// p->lock must be held when using these:
		enum ProcState _state;        // Process state
		void *_chan;                  // If non-zero, sleeping on chan
		int _killed;                  // If non-zero, have been killed
		int _xstate;                  // Exit status to be returned to parent's wait
		int _pid;                     // Process ID

		// wait_lock must be held when using this:
		Pcb *parent;         // Parent process

		// these are private to the process, so p->lock need not be held.
		uint64 _kstack = 0;               // Virtual address of kernel stack
		uint64 _sz;                   // Size of process memory (bytes)
		mm::PageTable _pt;    // User lower half address page table
		TrapFrame *_trapframe; // data page for uservec.S, use DMW address
		Context _context;      // swtch() here to run process
		// struct file *ofile[ NOFILE ];  // Open files
		// struct inode *cwd;           // Current directory
		char _name[ 16 ];               // Process name (debugging)

		int _slot;					// Rest of time slot 
		int _priority;				// Process priority (0-20)=(highest-lowest)

		// about share memory 
		uint _shm;					// The low-boundary address of share memory 
		void *_shmva[ SHM_NUM ];				// The sharemem of process
		uint _shmkeymask;			// The mask of using shared physical memory page 

		//about msg queue 
		uint _mqmask;

		// vm
		struct vma *vm[ 10 ];  // virtual memory area <<<<<<<<<<<<<<<<<< what??? Could ONE process has several vm space?

	public:
		Pcb() {};
		void init( const char *lock_name, uint gid );
		void map_kstack( mm::PageTable &pt );

		int get_priority();

	public:
		Context *get_context() { return &_context; }
		// smp::Lock &get_lock() { return _lock; }		<<<<<<<<<<<<<<<<<< 注意任何时候都不要尝试把任何的类的私有lock返回出去，
																		// lock不正当的使用会带来问题，
																		// 外部需要申请这个类的资源时应当在类中实现一个返回资源的接口,
																		// 而lock的使用应当在接口中由类内部来决定
		fs::Dentry *get_cwd() { return _cwd; }
		uint get_pid() { return _pid; }
		TrapFrame* get_trapframe() { return _trapframe; }
		uint64 get_kstack() { return _kstack; }
		mm::PageTable get_pagetable() { return _pt; }
		ProcState get_state() { return _state; }

		void set_trapframe( TrapFrame * tf ) { _trapframe = tf; }

		bool is_killed() { return _killed != 0; }
	};

	extern Pcb k_proc_pool[ num_process ];
}