//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/buddy_allocator.hh"
#include "mm/page_table.hh"
#include "mm/physical_memory_manager.hh"
#include "mm/virtual_memory_manager.hh"
#include "klib/common.hh"

namespace mm
{
// ________________________________________________________________
// >>>> class Buddy Allocator 

// -------- public interface --------

	void BuddyAllocator::init( const char *lock_name, PageTable *page_table, void *heap_start, uint64 heap_size )
	{
		_lock.init( lock_name );
		_page_table = page_table;

		assert( ( ( uint64 ) heap_start % mm::PageEnum::pg_size ) == 0, "heap start address should be aligned to page. page size : %d bytes", mm::pg_size );
		assert( heap_size > 4 * mm::pg_size, "heap size too small. page size : %d bytes", mm::pg_size );
		assert( ( heap_size & ( heap_size - 1 ) ) == 0, "heap size should be a power of 2. input heap size in hex : %xh", mm::pg_size );
		assert( heap_size <= buddy_max_heap_size, "heap size too big. max size is %d MiB.", buddy_max_heap_size / _1M );

		_heap_addr = heap_start;
		_heap_size = heap_size;

		// one heap page is correspendent to one node 
		uint heap_page_cnt = heap_size / mm::pg_size;

		// init bnode manager

		_node_manager.init( "buddy node manager", heap_page_cnt );

		// init virtual memory 

		uint64 pa;
		uint64 va = ( uint64 ) heap_start;
		for ( uint i = 0; i < heap_page_cnt; i++, va += mm::pg_size )
		{
			pa = ( uint64 ) mm::k_pmm.alloc_page();
			mm::k_vmm.map_pages( *_page_table, va, mm::pg_size, pa,
				( loongarch::PteEnum::presence_m ) |
				( loongarch::PteEnum::writable_m ) |
				( loongarch::PteEnum::dirty_m ) |
				( 0 << loongarch::PteEnum::plv_s ) |							// 特权级0
				( loongarch::MatEnum::mat_cc << loongarch::PteEnum::mat_s )		// 强序可缓存 
			);
		}

		// init bnodes map 

		for ( auto &nd : _bnodes_map )
			nd = nullptr;
		for ( auto &st : _bnodes_state_map )
			st = 0;

		// init the first area ( the whole heap memory )

		BuddyNode * bnode = _node_manager.allocate_node();
		assert( bnode != nullptr, "buddy-alloc : init fail due to no buddy-node to use." );
		_set_node( bnode, ( uint64 ) heap_start, heap_size, _size_to_order( heap_size ) );
		_record_node_in_map( bnode, false );
	}

	void *BuddyAllocator::allocate_pages( uint count )
	{
		if ( count == 0 )
		{
			log_warn( "buddy-alloc : request ZERO pages. return null pointer." );
			return nullptr;
		}
		if ( count > buddy_max_heap_pages_cnt )
		{
			log_warn(
				"buddy-alloc : request too many pages.\n"
				">> maximum is %d, but input %d.\n"
				">> return null pointer.",
				buddy_max_heap_pages_cnt, count
			);
			return nullptr;
		}

		// get index in list array
		// 2^(order-1) < count <= 2^(order)
		uint order = ( count & ( count - 1 ) ) == 0 ?
			lowest_bit( count ) :		// allocate a complete area
			highest_bit( count ) + 1;	// allocate an area a little bigger than the size requested 

		// find available area 
		uint index = order;
		for ( ; index < _node_manager._node_list_array_max_length; ++index )
		{
			if ( _node_manager.list_has_availble_area( index ) )
				break;
		}
		

		BuddyNode *node;
		node = _node_manager.pop_available_node( index );
		// if gotten area is too big, then split the area
		while ( index != order )
		{
			index--;
			BuddyNode *split_node = _node_manager.allocate_node();

			node->_area_size >>= 1;
			_set_node( split_node, node->_area_start + node->_area_size, node->_area_size, index );
			_record_node_in_map( split_node, false );
		}

		// area size is fit now
		// record the area for freeing it in the future
		_record_node_in_map( node, true );

		// return the pointer pointing at the area
		return ( void* ) node->_area_start;
	}

	void BuddyAllocator::free_pages( void *ptr )
	{
		uint64 index = _page_index_in_heap( ptr );
		MappedNodeState node_state = _get_node_state( index );
		if ( node_state == bnode_not_exist || node_state == bnode_available )
			return;		// not exist or freed

		BuddyNode * node = _bnodes_map[ index ];

		// unmap node 
		_remove_node_in_map( index );

		// get buddy area
		uint64 bindex = _page_index_in_heap( ( void* ) ( node->_area_start ^ node->_area_size ) );
		BuddyNode * bnode = _bnodes_map[ bindex ];
		assert( bnode != nullptr,
			"buddy-alloc : free an area with a not-exist buddy area\n"
			">> possible reason -> buddy-allocator not map buddy node while allocate an area"
		);

		// while buddy area is available 
		while ( _get_node_state( bindex ) == bnode_available )
		{
			_remove_node_in_map( bindex );				// unmap buddy area
			_node_manager.free_node( bnode );	// remove the bnode in list

			// combine buddy nodes
			node->_area_start = node->_area_start > bnode->_area_start ?
				bnode->_area_start : node->_area_start;
			node->_area_size <<= 1;

			// get maximum area, no area to combine, break
			if ( node->_area_size == _heap_size )
				break;

			// refind buddy area
			bindex = _page_index_in_heap( ( void* ) ( node->_area_start ^ node->_area_size ) );
			bnode = _bnodes_map[ bindex ];
			assert( bnode != nullptr,
				"buddy-alloc : free an area with a not-exist buddy area\n"
				">> possible reason -> buddy-allocator not map buddy node while allocate an area"
			);
		}

		// buddy area is busy, so insert current node into availble list
		_node_manager.insert_available_list( node, _size_to_order( node->_area_size) );

		// remap node
		_record_node_in_map( node, bnode_available );
	}

// -------- private helper function --------

	void BuddyAllocator::_set_node( BuddyNode *node, uint64 area, uint64 size, int order )
	{
		assert( area >= ( uint64 ) _heap_addr, "area starts address lower than heap address! heap-addr : %p", _heap_addr );
		assert( area + size <= ( uint64 ) _heap_addr + _heap_size,
			"area beyond heap memory boundary!\n"
			">> heap-boudary : %p -- %p",
			( uint64 ) _heap_addr,
			( uint64 ) _heap_addr + _heap_size );
		assert( size > mm::pg_size, "size too small to cover a page. size = %d bytes", size );
		assert( ( size & ( size - 1 ) ) == 0, "size shall be a power of 2. size : %xh", size );
		assert( order < ( int ) _node_manager._node_list_array_max_length, "get fault order, maybe common algorithm error." );

		node->_area_start = area;
		node->_area_size = size;

		_node_manager.insert_available_list( node, order );
	}

} // namespace mm
