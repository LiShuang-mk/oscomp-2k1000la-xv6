//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/physical_memory_manager.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/buddy_algorithmn/buddy_allocator.hh"

#include "klib/common.hh"

#include <hsai_global.hh>
#include <mem/virtual_memory.hh>
#include <mem/page.hh>

namespace mm
{
	// /// @brief This struct just be used to unused physical page;
	// /// @details A simple link list connect all unused page;
	// struct PageHead
	// {
	// 	struct PageHead *_next;
	// };

	PhysicalMemoryManager k_pmm;

	static BuddyAllocator k_phy_alloc;

	PhysicalMemoryManager::PhysicalMemoryManager( const char *name )
		: PhysicalMemoryManager()
	{
		_lock.init( name );
		_ram_base = hsai::k_mem->mem_start();
		_ram_end = _ram_base + hsai::k_mem->mem_size();
		_free_list = nullptr;
		_free_range( ( void* ) _ram_base, ( void * ) _ram_end );

		new ( &k_phy_alloc ) BuddyAllocator( ( void * ) _ram_base, _ram_end - _ram_base );
		_allocator = ( VirtualPageAllocator * ) &k_phy_alloc;
	}

	void * PhysicalMemoryManager::alloc_pages( ulong cnt )
	{
		void * pgs = _allocator->alloc_pages( cnt );
		if ( pgs == nullptr )
			return pgs;
		_fill_pages( pgs, cnt, alloc_junk );
		return pgs;
	}

	int PhysicalMemoryManager::free_pages( void *pa )
	{
		// _free_page( pa );
		if ( pa == nullptr )
			return -1;
		int free_cnt = _allocator->free_pages( pa );
		_fill_pages( pa, free_cnt, freed_junk );
		return 0;
	}

	void *PhysicalMemoryManager::alloc_page()
	{
		// return _alloc_page();
		void * pg = _allocator->alloc_pages( 1 );
		if ( pg == nullptr )
			return pg;
		_fill_junk( pg, alloc_junk );
		return pg;
	}

	void PhysicalMemoryManager::clear_page( void *pa )
	{
		_fill_junk( pa, MemJunk::null_junk );
	}

	void PhysicalMemoryManager::clear_pages( void *pa, ulong cnt )
	{
		ulong a = ( ulong ) pa;
		for ( ; cnt > 0; a += hsai::page_size, cnt-- )
			_fill_junk( ( void* ) a, MemJunk::null_junk );
	}

////////////////////// private helper functions /////////////////////////////////


	void PhysicalMemoryManager::_free_range( void *pa_start, void *pa_end )
	{
		char *p;
		p = ( char * ) hsai::page_round_up( ( uint64 ) pa_start );
		int page_cnt = 0;
		for ( ; p + hsai::page_size <= ( char * ) pa_end; p += hsai::page_size )
		{
			_fill_junk( p, freed_junk );
			page_cnt++;
		}
		log_trace(
			"物理页式内存起始地址 : %p\n"
			"物理页式内存长度     : %d MiB\n"
			"物理内存页数量       : %d",
			( uint64 ) pa_start,
			( ( uint64 ) pa_end - ( uint64 ) pa_start ) / _1M,
			page_cnt
		);
	}

	void PhysicalMemoryManager::_fill_junk( void * pa, MemJunk mj )
	{
		uint64 *p = ( uint64 * ) pa;
		const uint cnt = ( uint ) hsai::page_size >> 3;
		for ( uint i = 0; i < cnt; i++ )
			p[ i ] = mj;
	}

	void PhysicalMemoryManager::_fill_pages( void * pages, uint cnt, MemJunk mj )
	{
		MemJunk * end_unit = ( MemJunk * ) ( ( ulong ) pages + cnt * hsai::page_size );
		for ( MemJunk * unit = ( MemJunk * ) pages; unit < end_unit; ++unit )
			*unit = mj;
	}

// debug function

	void PhysicalMemoryManager::debug_test_buddy()
	{
		BuddyAllocator * ba = ( BuddyAllocator * ) _allocator;
		ba->debug_print();

		uint pgcnt;
		void * pg;

		pgcnt = 1;
		pg = alloc_pages( pgcnt );
		log_trace( "test alloc %d pages : %p", pgcnt, pg );
		ba->debug_print();
		free_pages( pg );
		log_trace( "test free  %d pages : %p", pgcnt, pg );
		ba->debug_print();

		pgcnt = 10;
		pg = alloc_pages( pgcnt );
		log_trace( "test alloc %d pages : %p", pgcnt, pg );
		ba->debug_print();
		free_pages( pg );
		log_trace( "test free  %d pages : %p", pgcnt, pg );
		ba->debug_print();

		pgcnt = 100;
		pg = alloc_pages( pgcnt );
		log_trace( "test alloc %d pages : %p", pgcnt, pg );
		ba->debug_print();
		free_pages( pg );
		log_trace( "test free  %d pages : %p", pgcnt, pg );
		ba->debug_print();

		pgcnt = 1000;
		pg = alloc_pages( pgcnt );
		log_trace( "test alloc %d pages : %p", pgcnt, pg );
		ba->debug_print();
		free_pages( pg );
		log_trace( "test free  %d pages : %p", pgcnt, pg );
		ba->debug_print();

		void * ps[ 1000 ];

		pgcnt = 10;
		int i = 0;
		for ( auto & p : ps )
		{
			p = alloc_pages( pgcnt );
			printf( "%d ", i );
			++i;
		}
		printf( "\n" );
		log_trace( "test alloc %d * 1000 pages : [0]=%p", pgcnt, ps[ 0 ] );
		ba->debug_print();
		for ( auto & p : ps )
			free_pages( p );
		log_trace( "test free  %d * 1000 pages : [0]=%p", pgcnt, ps[ 0 ] );
		ba->debug_print();

	}

	void PhysicalMemoryManager::debug_print()
	{
		k_phy_alloc.debug_print();
	}

} // namespace mm