//
// Created by Li Shuang ( pseudonym ) on 2024-06-27
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "smp/spin_lock.hh"
#include "virtual_cpu.hh"
#include "hsai_global.hh"
#include "hsai_log.hh"

namespace hsai
{
	SpinLock::SpinLock()
	{

	}

	void SpinLock::init( const char * name )
	{
		_name = name;
		_locked = nullptr;
	}

	void SpinLock::acquire()
	{
		VirtualCpu * cpu = get_cpu();
		cpu->push_interrupt_off();

		if ( is_held() )
			hsai_panic( "lock is already held." );
		
		std::atomic_thread_fence( std::memory_order::acq_rel );

		VirtualCpu * expected = nullptr;
		while ( _locked.compare_exchange_strong( expected, cpu, std::memory_order::acq_rel ) == false )
			expected = nullptr;
	}

	void SpinLock::release()
	{
		if ( !is_held() )
			hsai_panic( "lock is already released." );
		_locked.store( nullptr, std::memory_order::acq_rel );

		std::atomic_thread_fence( std::memory_order::acq_rel );

		VirtualCpu * cpu = get_cpu();
		cpu->pop_intterupt_off();
	}

	bool SpinLock::is_held()
	{
		return ( _locked.load() == get_cpu() );
	}

} // namespace hsai
