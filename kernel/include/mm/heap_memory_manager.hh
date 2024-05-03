//
// Created by Li Shuang ( pseudonym ) on 2024-04-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "mm/buddy_allocator.hh"
#include "klib/liballoc_allocator.hh"
#include "klib/buddy_page_alloc.hh"

namespace mm
{
	class HeapMemoryManager
	{
	private:
		smp::Lock _lock;

		BuddyAllocator _k_allocator_coarse;

		kernellib::L_Allocator _k_allocator_fine;

	public:
		HeapMemoryManager() {};
		void init( const char *lock_name );

		void *allocate( uint64 size );

		void free( void *p );
	};

	extern HeapMemoryManager k_hmm;
} // namespace mm
