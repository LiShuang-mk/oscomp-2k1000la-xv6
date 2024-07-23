//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/buddy_algorithmn/buddy_allocator.hh"
#include "mm/page_table.hh"
// #include "mm/physical_memory_manager.hh"
// #include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"

namespace mm
{
	BuddyAllocator::BuddyAllocator()
		: _mem_start( nullptr )
		, _mem_size( 0 )
		, _buddy_nodes_cnt( 0 )
		, _buddy_managers_cnt( 0 )
	{

	}

	BuddyAllocator::BuddyAllocator( const char * lock_name, ulong mem_start, ulong mem_size )
		: BuddyAllocator()
	{
		assert( ( ( uint64 ) mem_start % hsai::page_size ) == 0, "" );
		assert( mem_size >= buddy_mem_min_size, "" );
		assert( ( mem_size % buddy_mem_min_size ) == 0, "" );
		assert( mem_size <= buddy_mem_max_size, "" );

		_mem_start = ( void * ) mem_start;
		_mem_size = mem_size;
		_lock.init( lock_name );

		// clear buddy node

		for ( BuddyNode &node : _buddy_nodes )
		{
			node = { ._area_start = 0, ._area_size = 0 };
		}

		// init buddy node list and
		// split heap into several buddy area

		ulong l = 1;	// bit pointer
		uint t = 1;		// node pointer
		uint n = 0;		// node number index
		uint i = 0;		// list index
		ulong a = ( ulong ) _mem_start;	// address index
		ulong pg_cnt = _mem_size / hsai::page_size;
		while ( l != 0 )
		{
			if ( ( l & pg_cnt ) != 0 )
			{
				new ( &_buddy_managers[ i ] ) BuddyManager( a, &_buddy_nodes[ n ], t );
				// printf( "(%u,%u,%#lx)\n", n, t, a );
				n += t;
				a += l * hsai::page_size;
				i++;
			}
			l <<= 1;
			t++;
		}
		assert( i <= buddy_manager_max_cnt, "too many buddy managers" );
		assert( n <= buddy_node_max_cnt, "too many buddy nodes" );
		_buddy_managers_cnt = i;
		_buddy_nodes_cnt = n;

		// init bnodes map 

		for ( auto &od : _order_of_used_node )
			od = 0;
	}

	void * BuddyAllocator::alloc_pages( uint count )
	{
		if ( count == 0 )
		{
			printf( "buddy-alloc : request ZERO pages. return null pointer." );
			return nullptr;
		}


		// get the order of the node including 'count' pages
		// 2^(order-1) < count <= 2^(order)
		uint16 order = ( count & ( count - 1 ) ) == 0 ?
			( uint16 ) lowest_bit( count ) :		// allocate a complete area
			( uint16 ) highest_bit( count ) + 1;	// allocate an area a little bigger than the size requested 

		_lock.acquire();


		BuddyNode nd;
		for ( uint i = 0; i < _buddy_managers_cnt; ++i )
		{
			auto &bm = _buddy_managers[ i ];
			nd = bm.allocate_node( order );
			if ( nd._area_size != 0 )
			{
				_record_order( nd, order );
				break;
			}
		}
		_lock.release();

		void * ptr = ( void * ) nd._area_start;

		return ptr;
	}

	int BuddyAllocator::free_pages( void * ptr )
	{
		uint64 index = _page_index_in_mem( ptr );
		_lock.acquire();
		uint8 order = _order_of_used_node[ index ];
		_lock.release();
		BuddyNode node = { ._area_start = ( ulong ) ptr, ._area_size = hsai::page_size << order };

		for ( auto &bm : _buddy_managers )
		{
			if ( bm.has_node( node ) )
			{
				bm.recycle_node( order, node );
				break;
			}
		}

		return 1 << order;
	}

	static int debug_cnt = 0;
#ifndef COLOR_PRINT
#define COLOR_PRINT

#define BLUE_COLOR_PRINT "\033[36m"
#define CLEAR_COLOR_PRINT "\033[0m"

#endif 

	void BuddyAllocator::debug_print_node_list()
	{
		printf( BLUE_COLOR_PRINT "[%d]\t\n" CLEAR_COLOR_PRINT, debug_cnt );
		for ( auto &bm : _buddy_managers )
		{
			bm._debug_print_node();
		}
		++debug_cnt;
	}

} // namespace mm
