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

struct TrapFrame;

enum ProcState
{
	UNUSED,
	USED,
	SLEEPING,
	RUNNABLE,
	RUNNING,
	ZOMBIE
};

class Pcb
{
private:
	smp::Lock _lock;

	// p->lock must be held when using these:
	enum ProcState _state;        // Process state
	void *_chan;                  // If non-zero, sleeping on chan
	int _killed;                  // If non-zero, have been killed
	int _xstate;                  // Exit status to be returned to parent's wait
	int _pid;                     // Process ID

	// wait_lock must be held when using this:
	Pcb *parent;         // Parent process

	// these are private to the process, so p->lock need not be held.
	uint64 _kstack;               // Virtual address of kernel stack
	uint64 _sz;                   // Size of process memory (bytes)
	PageTable _pt;    // User lower half address page table
	struct TrapFrame *_trapframe; // data page for uservec.S, use DMW address
	struct Context _context;      // swtch() here to run process
	// struct file *ofile[ NOFILE ];  // Open files
	// struct inode *cwd;           // Current directory
	char _name[ 16 ];               // Process name (debugging)

	int _slot;					// Rest of time slot 
	int _priority;				// Process priority (0-20)=(highest-lowest)

	// about share memory 
	uint _shm;					// The low-boundary address of share memory 
	uint _shmkeymask;			// The mask of using shared physical memory page 
	// void *_shmva[ MAX_SHM_NUM ];			// The starting virtual address list of shared memory 

	//about msg queue 
	uint _mqmask;

public:
	Pcb() {};
	void init( const char *lock_name );
};