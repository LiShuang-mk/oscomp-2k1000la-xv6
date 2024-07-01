//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>

namespace mm
{
	struct PageHead;

	enum MemJunk : uint64
	{
		freed_junk = 0x0101010101010101UL,
		alloc_junk = 0x0505050505050505UL,
		null_junk = 0x0UL
	};

	class PhysicalMemoryManager
	{
	private:
		hsai::SpinLock _lock;
		PageHead *_free_list = nullptr;
		uint64 _ram_base;
		uint64 _ram_end;
		uint64 _trace_page_cnt;

	public:
		PhysicalMemoryManager() {};
		void init( const char *name, uint64 ram_base, uint64 ram_end );
		void free_page( void *pa );
		void *alloc_page();
		void clear_page( void *pa );

		inline uint64 trace_free_pages_count() { return _trace_page_cnt; }

	private:
		void _free_range( void *pa_start, void *pa_end );
		void _free_page( void *pa );
		void *_alloc_page();
		void _fill_junk( void *pa, MemJunk mj );
	};


	extern PhysicalMemoryManager k_pmm;
}