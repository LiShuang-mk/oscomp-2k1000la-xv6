//
// Created by Li shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

#include <smp/spin_lock.hh>

namespace pm
{
	class SleepLock
	{
	private:
		bool _locked = false;
		hsai::SpinLock _lock;
		uint _pid;
		// for debugging 
		const char *_name;
	public:
		SleepLock() {};
		void init( const char *lock_name, const char *name );
		void acquire();
		void release();
		bool is_holding();
	};
} // namespace pm
