//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>

#include "virtual_page_allocator.hh"

namespace mm
{
	struct PageHead;

	enum MemJunk : uint64
	{
		freed_junk = 0x0101010101010101UL,
		alloc_junk = 0x0A110CA110CA110CUL,
		null_junk = 0x0UL
	};

	class PhysicalMemoryManager : public VirtualPageAllocator
	{
	private:
		hsai::SpinLock _lock;
		PageHead *_free_list = nullptr;
		uint64 _ram_base;
		uint64 _ram_end;
		uint64 _trace_page_cnt;
		VirtualPageAllocator * _allocator;

	public:
		PhysicalMemoryManager() = default;
		PhysicalMemoryManager( const char * name );

		virtual void *alloc_pages( uint cnt ) override;
		virtual int free_pages( void *pa ) override;

		void *alloc_page();
		void clear_page( void *pa );

		inline uint64 trace_free_pages_count() { return _trace_page_cnt; }

	private:
		void _free_range( void *pa_start, void *pa_end );
		// void _free_page( void *pa );
		// void *_alloc_page();
		void _fill_junk( void *pa, MemJunk mj );
		void _fill_pages( void * pages, uint cnt, MemJunk mj );

	public:
		void debug_test_buddy();
	};


	extern PhysicalMemoryManager k_pmm;
}