//
// Created by Li shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "pm/sleep_lock.hh"
#include "pm/process_manager.hh"
#include "klib/common.hh"

namespace pm
{
	void SleepLock::init( const char *lock_name, const char *name )
	{
		_lock.init( lock_name );
		_name = name;
		_locked = false;
		_pid = 0;
	}

	void SleepLock::acquire()
	{
		_lock.acquire();
		while ( _locked )
			log_error( "sleep not implement" );
		_locked = true;
		_pid = pm::k_pm.get_cur_pcb()->get_pid();
		_lock.release();
	}

	void SleepLock::release()
	{
		_lock.acquire();
		_locked = 0;
		_pid = 0;
		log_error( "wake up not implement" );
		_lock.release();
	}

	bool SleepLock::is_holding()
	{
		bool held;
		_lock.acquire();
		held = _locked && ( _pid == pm::k_pm.get_cur_pcb()->get_pid() );
		_lock.release();
		return held;
	}
} // namespace pm
