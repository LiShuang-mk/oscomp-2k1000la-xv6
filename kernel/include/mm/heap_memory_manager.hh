//
// Created by Li Shuang ( pseudonym ) on 2024-04-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/liballoc_algorithmn/liballoc_allocator.hh"

#include <smp/spin_lock.hh>

namespace mm
{
	class HeapMemoryManager
	{
	private:
		hsai::SpinLock _lock;

		// BuddyAllocator _k_allocator_coarse;

		L_Allocator _allocator;

	public:
		HeapMemoryManager() {};
		void init( const char *lock_name );

		void *allocate( uint64 size );

		void free( void *p );
	};

	extern HeapMemoryManager k_hmm;
} // namespace mm
