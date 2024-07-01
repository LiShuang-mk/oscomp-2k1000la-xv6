//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/page_table.hh"
#include "pm/context.hh"
#include "pm/sharemem.hh"

#include <smp/spin_lock.hh>

#include <EASTL/string.h>

namespace fs
{
	class Dentry;

	struct xv6_file;
}
namespace pm
{
	struct TrapFrame;

	constexpr uint num_process = 32;
	constexpr int default_proc_prio = 10;
	constexpr int lowest_proc_prio = 19;
	constexpr int highest_proc_prio = 0;
	constexpr uint max_open_files = 128;

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
	public:
		hsai::SpinLock _lock;
		int _gid = num_process;					// global ID in pool 

		fs::Dentry *_cwd;				// current working directory
		eastl::string _cwd_name;
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
		fs::xv6_file *_ofile[ max_open_files ];  // Open files
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

		uint64 _start_tick;			// the tick value when the process starts running
		uint64 _user_ticks;			// the ticks of that the process has run in user mode
		uint64 _last_user_tick;		// the tick value of that the process returns to user mode last time

		uint64 _hp;					// 一个假的堆指针，后续会删除

		// vm
		struct vma *vm[ 10 ];  // virtual memory area <<<<<<<<<<<<<<<<<< what??? Could ONE process has several vm space?

	public:
		Pcb();
		void init( const char *lock_name, uint gid );
		void map_kstack( mm::PageTable &pt );

		int get_priority();

	public:
		Context *get_context() { return &_context; }
		// smp::Lock &get_lock() { return _lock; }		<<<<<<<<<<<<<<<<<< 注意任何时候都不要尝试把任何的类的私有lock返回出去，
																		// lock不正当的使用会带来问题，
																		// 外部需要申请这个类的资源时应当在类中实现一个返回资源的接口,
																		// 而lock的使用应当在接口中由类内部来决定
	public:
		fs::Dentry *get_cwd() { return _cwd; }
		void kill() { _killed = 1; }
		Pcb * get_parent() { return parent; }
		void set_state( ProcState state ) { _state = state; }
		void set_xstate( int xstate ) { _xstate = xstate; }
		size_t get_sz() { return _sz; }
		//void set_chan(void *chan) { _chan = chan; }
		uint get_pid() { return _pid; }
		uint get_ppid() { return parent ? parent->_pid : 0; }
		TrapFrame* get_trapframe() { return _trapframe; }
		uint64 get_kstack() { return _kstack; }
		mm::PageTable get_pagetable() { return _pt; }
		ProcState get_state() { return _state; }
		char * get_name() { return _name; }
		uint64 get_size() { return _sz; }
		uint64 get_last_user_tick() { return _last_user_tick; }
		uint64 get_user_ticks() { return _user_ticks; }
		fs::xv6_file * get_open_file( int fd )
		{
			if ( fd < 0 || fd >= ( int ) max_open_files ) return nullptr;
			return _ofile[ fd ];
		}

		void set_trapframe( TrapFrame * tf ) { _trapframe = tf; }

		void set_last_user_tick( uint64 tick ) { _last_user_tick = tick; }
		void set_user_ticks( uint64 ticks ) { _user_ticks = ticks; }

		bool is_killed() { return _killed != 0; }
	};

	extern Pcb k_proc_pool[ num_process ];
}