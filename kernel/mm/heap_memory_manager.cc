//
// Created by Li Shuang ( pseudonym ) on 2024-04-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/heap_memory_manager.hh"
#include "mm/page_table.hh"
#include "mm/memlayout.hh"


namespace mm
{
	HeapMemoryManager k_hmm;

	void HeapMemoryManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		_k_allocator.init(
			"kernel heap allocator",
			&k_pagetable,
			( void* ) vml::vm_kernel_heap_start,
			vml::vm_kernel_heap_size
		);
	}

	void * HeapMemoryManager::allocate( uint64 size )
	{
		if ( size < pg_size )
		{
			log_warn(
				"dynamic memory allocation not implement.\n"
				"too small memory allocating will not success.\n"
				">> request size = %d bytes", size
			);
			return nullptr;
		}
		log_trace( "申请内存分配 %d bytes", size );
		return _k_allocator.allocate_pages( ( size + pg_size - 1 ) / pg_size );
	}

	void HeapMemoryManager::free( void *p )
	{
		log_trace( "内存释放" );
		_k_allocator.free_pages( p );
	}
} // namespace mm
