#pragma once
#include "smp/lock.hh"
#include "pm/process.hh"

namespace pm
{

	class Scheduler
	{
	private:
		smp::Lock _sche_lock;
	public:
		Scheduler() = default;
		void init( const char *name );
		void add_thread();
		void remove_thread();
		void switch_to_proc(pm::Pcb *p);
		int  get_highest_proirity();
		void start_schedule();
		// void switch_to_proc( pm::Pcb *p );

		void yield();
		void call_sched();
	};

	extern Scheduler k_scheduler;
}