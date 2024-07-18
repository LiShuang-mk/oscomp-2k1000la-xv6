//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "buddy.hh"
#include "buddy_manager.hh"
#include "mm/virtual_page_allocator.hh"
#include "klib/common.hh"

#include <smp/spin_lock.hh>

namespace mm
{
	class PageTable;

	class BuddyAllocator : public VirtualPageAllocator
	{
	private:
		hsai::SpinLock _lock;
		void *_mem_start;
		uint64 _mem_size;

		BuddyNode _buddy_nodes[ buddy_node_max_cnt ];
		uint _buddy_nodes_cnt;

		BuddyManager _buddy_managers[ buddy_manager_max_cnt ];
		uint _buddy_managers_cnt;

		enum MappedNodeState
		{
			bnode_not_exist,
			bnode_available,
			bnode_busy,
		};

		// record the order of each node in use
		uint8 _order_of_used_node[ buddy_pages_max_cnt ];

	public:
		BuddyAllocator();
		BuddyAllocator( const char * lock_name, ulong mem_start, ulong mem_size );

		virtual void * alloc_pages( uint cnt ) override;

		virtual int free_pages( void * ptr ) override;

	private:
		// map node to area and insert node into the node list
		void _set_node( BuddyNode *node, uint64 area, uint64 size, int order )
		{
			assert( area >= ( uint64 ) _mem_start, "" );
			assert( area + size <= ( uint64 ) _mem_start + _mem_size, "" );
			assert( size >= hsai::page_size, "" );
			assert( ( size & ( size - 1 ) ) == 0, "" );
			// assert( order < ( int ) _node_manager._node_list_array_max_length );

			node->_area_start = area;
			node->_area_size = size;

			// _node_manager.insert_available_list( node, order );
		}

		void _record_order( BuddyNode nd, uint8 order )
		{
			_order_of_used_node[ _page_index_in_mem( ( void * ) nd._area_start ) ] = order;
		}

		void _erase_order( BuddyNode nd )
		{
			_order_of_used_node[ _page_index_in_mem( ( void * ) nd._area_start ) ] = 0;
		}

		inline uint64 _page_index_in_mem( void *ptr )
		{
			return ( ( uint64 ) ptr - ( uint64 ) _mem_start ) / hsai::page_size;
		}

		inline int _size_to_order( uint64 size )
		{
			return lowest_bit( size ) - hsai::page_size_shift;
		}

		inline void *_get_buddy_start_addr( uint64 ptr, uint64 area_size )
		{
			assert( ptr >= ( uint64 ) _mem_start, "" );
			return ( void * ) ( ( ( ptr - ( uint64 ) _mem_start ) ^ area_size ) + ( uint64 ) _mem_start );
		}

	public:
		void debug_print_node_list();
	};


} // namespace mm
