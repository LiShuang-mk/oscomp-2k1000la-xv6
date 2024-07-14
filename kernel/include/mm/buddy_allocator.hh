//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/buddy.hh"
#include "klib/common.hh"

#include <smp/spin_lock.hh>

namespace mm
{
	class PageTable;

	class BuddyAllocator
	{
	private:
		hsai::SpinLock _lock;
		PageTable *_page_table;
		void *_heap_addr;
		uint64 _heap_size;

		BuddyNodeManager _node_manager;

		enum MappedNodeState
		{
			bnode_not_exist,
			bnode_available,
			bnode_busy,
		};

		// used to find bnode from pointer
		BuddyNode * _bnodes_map[ buddy_max_heap_pages_cnt ];

		// bitmap indicating the bnode is weather busy or available ( 1 indicates busy )
		uint64 _bnodes_state_map[ buddy_max_heap_pages_cnt / 64 ];

	public:
		BuddyAllocator() {};
		void init( const char *lock_name, PageTable *page_table, void *heap_start, uint64 heap_size );

		void * allocate_pages( uint count );

		void free_pages( void * ptr );

	private:
		// map node to area and insert node into the node list
		void _set_node( BuddyNode *node, uint64 area, uint64 size, int order );

		inline uint64 _page_index_in_heap( void *ptr )
		{
			return ( ( uint64 ) ptr - ( uint64 ) _heap_addr ) / hsai::page_size;
		}

		inline void _record_node_in_map( BuddyNode *bnode, uint64 index, bool is_busy )
		{
			_bnodes_map[ index ] = bnode;
			is_busy ?
				bit_set( ( void* ) _bnodes_state_map, index ) :
				bit_reset( ( void* ) _bnodes_state_map, index );
		}
		inline void _record_node_in_map( BuddyNode *bnode, bool is_busy )
		{
			uint64 index = _page_index_in_heap( ( void* ) bnode->_area_start );
			_record_node_in_map( bnode, index, is_busy );
		}

		inline void _remove_node_in_map( uint64 index )
		{
			_bnodes_map[ index ] = nullptr;
		}
		inline void _remove_node_in_map( BuddyNode *bnode )
		{
			uint64 index = _page_index_in_heap( ( void* ) bnode->_area_start );
			_remove_node_in_map( index );
		}

		inline MappedNodeState _get_node_state( uint64 index )
		{
			if ( _bnodes_map[ index ] == nullptr )
				return bnode_not_exist;
			return bit_test( ( void* ) _bnodes_state_map, index ) ?
				bnode_busy :
				bnode_available;
		}

		inline int _size_to_order( uint64 size )
		{
			return lowest_bit( size ) - hsai::page_size_shift;
		}

		inline void * _get_buddy_start_addr( uint64 ptr, uint64 area_size )
		{
			assert( ptr >= ( uint64 ) _heap_addr, "[Buddy] not valid heap ptr. ptr: %x", ptr );
			return ( void * ) ( ( ( ptr - ( uint64 ) _heap_addr ) ^ area_size ) + ( uint64 ) _heap_addr );
		}
	};

} // namespace mm
