//
// Created by Li Shuang ( pseudonym ) on 2024-04-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/heap_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/page_table.hh"
#include "mm/memlayout.hh"

#include "klib/common.hh"


namespace mm
{
	HeapMemoryManager k_hmm;

	void HeapMemoryManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		// _k_allocator_coarse.init(
		// 	"kernel heap allocator - buddy",
		// 	&k_pagetable,
		// 	( void* ) vml::vm_kernel_heap_start,
		// 	vml::vm_kernel_heap_size
		// );

		_allocator.init(
			"kernel heap allocator - liballoc",
			&k_pmm
		);
	}

	void * HeapMemoryManager::allocate( uint64 size )
	{
		// if ( size < pg_size )
		// {
		// 	log_warn(
		// 		"dynamic memory allocation not implement.\n"
		// 		"too small memory allocating will not success.\n"
		// 		">> request size = %d bytes", size
		// 	);
		// 	return nullptr;
		// }
		// log_trace( "申请内存分配 %d bytes", size );
		// return _k_allocator_coarse.allocate_pages( ( size + pg_size - 1 ) / pg_size );
		return _allocator.malloc( size );
	}

	void HeapMemoryManager::free( void *p )
	{
		// log_trace( "内存释放" );
		_allocator.free( p );
	}

#ifndef COLOR_PRINT
#define COLOR_PRINT

#define RED_COLOR_PINRT "\033[31m"
#define GREEN_COLOR_PRINT "\033[32m"
#define BLUE_COLOR_PRINT "\033[34m"
#define CYAN_COLOR_PINRT "\033[36m"
#define CLEAR_COLOR_PRINT "\033[0m"

#endif

	void HeapMemoryManager::print_heap_usage()
	{
		printf( GREEN_COLOR_PRINT "[heap usage]:\n" CLEAR_COLOR_PRINT );
		printf( BLUE_COLOR_PRINT );
		printf( "\tcached : %d Bytes\n", _allocator.get_cached_size() );
		printf( "\tused   : %d Bytes\n", _allocator.get_used_size() );
		printf( "\tmeta   : %d Bytes\n", _allocator.get_meta_size() );
		_allocator.debug_print();
	}
} // namespace mm
