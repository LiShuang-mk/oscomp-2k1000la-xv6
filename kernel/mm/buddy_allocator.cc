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
		assert( ( heap_size & ( heap_size - 1 ) ) == 0, "heap size should be a power of 2. input heap size in hex : %p", mm::pg_size );

		// init nodes list 

		_free_node_list._area_size = 0;
		_free_node_list._area_start = 0;
		_free_node_list._next = &_free_node_list;
		_free_node_list._prev = &_free_node_list;

		uint heap_page_cnt = heap_size / mm::pg_size;
		uint node_page_cnt = ( heap_page_cnt + node_per_page - 1 ) / node_per_page;

		BuddyNode *bnode;
		// full page with buddy-node
		for ( uint npi = 0; npi < node_page_cnt - 1; npi++ )
		{
			bnode = ( BuddyNode * ) mm::k_pmm.alloc_page();
			for ( uint i = 0; i < node_per_page; i++, bnode++ )
			{
				bnode->_area_size = bnode->_area_start = 0;
				// insert bnode into tail 
				_list_insert_tail( &_free_node_list, bnode );
			}
		}
		// last page with buddy-node 
		bnode = ( BuddyNode * ) mm::k_pmm.alloc_page();
		uint rest_node_cnt = heap_page_cnt % node_per_page;
		for ( uint i = 0; i < rest_node_cnt; i++, bnode++ )
		{
			bnode->_area_size = bnode->_area_start = 0;
			// insert bnode into tail 
			_list_insert_tail( &_free_node_list, bnode );
		}

		// init virtual memory 

		uint64 pa;
		uint64 va = ( uint64 ) heap_start;
		for ( uint i = 0; i < heap_page_cnt; i++, va += mm::pg_size )
		{
			pa = ( uint64 ) mm::k_pmm.alloc_page();
			mm::k_vmm.map_pages( *_page_table, va, mm::pg_size, pa,
				( loongarch::PteEnum::presence_m ) |
				( loongarch::PteEnum::writable_m ) |
				( 0 << loongarch::PteEnum::plv_s ) |							// 特权级0
				( loongarch::MatEnum::mat_cc << loongarch::PteEnum::mat_s )		// 强序可缓存 
			);
		}

		// init the first area 

		uint64 tmp = heap_size / mm::pg_size;
		uint _array_length = 0;
		while ( tmp != 0 )
		{
			tmp >>= 1;
			_array_length++;
		}
		_node_list_array = ( BuddyNode * ) mm::k_pmm.alloc_page();
		for ( uint i = 0; i < _array_length; ++i )
		{
			_node_list_array[ i ]._area_size = _node_list_array[ i ]._area_start = 0;
			_node_list_array[ i ]._next = _node_list_array[ i ]._prev = &_node_list_array[ i ];
		}

		bnode = _alloc_node();
		bnode->_area_start = ( uint64 ) heap_start;
		bnode->_area_size = heap_size;
		_list_insert_tail( &_node_list_array[ _array_length - 1 ], bnode );
	}

// -------- private helper function --------

	BuddyNode * BuddyAllocator::_alloc_node()
	{
		BuddyNode *bnode = _free_node_list._next;
		if ( bnode == &_free_node_list )
			log_panic( "buddy-allocator : no more nodes to use." );

		_list_remove_node( bnode );
		return bnode;
	}

} // namespace mm
