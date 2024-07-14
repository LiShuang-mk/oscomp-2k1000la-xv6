//
// Created by Li Shuang ( pseudonym ) on 2024-04-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/memlayout.hh"

#include <mem/page.hh>
#include <smp/spin_lock.hh>

namespace mm
{
	class BuddyAllocator;

	// The relation between buddy-node and heap area is one to one correspendence.
	// an area may include several pages or more 
	struct BuddyNode
	{
		uint64 _area_start;
		uint64 _area_size;
		BuddyNode *_prev;
		BuddyNode *_next;
	}__attribute__( ( __packed__ ) );

	constexpr uint node_per_page = hsai::page_size / sizeof( BuddyNode );

	// This struct is used to store buddy-node
	// due to the dynamic allocation of buddy-node
	struct BuddyUsedPageRecord
	{
		void *page;
		BuddyUsedPageRecord *next;
		BuddyUsedPageRecord *prev;
	};

	constexpr uint64 buddy_max_heap_size = mm::vml::vm_kernel_heap_size;
	constexpr uint64 buddy_max_heap_size_shift = [] () -> uint64
	{
		uint64 k = 0;
		for ( uint64 i = 1UL; ( i & buddy_max_heap_size ) == 0; i <<= 1 )
			k++;
		return k;
	}( );
	constexpr uint64 buddy_max_heap_pages_cnt = buddy_max_heap_size / hsai::page_size;

	static_assert( buddy_max_heap_size >= _1M, "buddy max heap size set too small" );
	static_assert( ( buddy_max_heap_size&( buddy_max_heap_size - 1 ) ) == 0, "buddy max heap size shall be 1MiB aligned" );

	class BuddyNodeManager
	{
		friend BuddyAllocator;
	private:
		hsai::SpinLock _lock;

		constexpr static uint _used_page_list_size = buddy_max_heap_pages_cnt / node_per_page;

		// physical pages used for buddy-node 
		BuddyUsedPageRecord _used_pages_records[ _used_page_list_size ];
		BuddyUsedPageRecord _used_pages_list_head;

		// used to allocate node 
		BuddyNode _free_node_list;

		constexpr static uint _node_list_array_max_length = buddy_max_heap_size_shift - hsai::page_size_shift + 1;

		// available area node list
		BuddyNode _node_list_array[ _node_list_array_max_length ];
		uint _array_length;

	public:
		BuddyNodeManager() {};
		void init( const char *lock_name, uint node_cnt );

		BuddyNode *allocate_node();

		void insert_available_list( BuddyNode *node, uint list_level );

		inline bool list_has_availble_area( uint list_level )
		{
			if ( list_level >= _node_list_array_max_length )
				return false;
			return !_list_is_empty( &_node_list_array[ list_level ] );
		}

		inline BuddyNode * pop_available_node( uint list_level )
		{
			if ( list_has_availble_area( list_level ) )
			{
				BuddyNode *bnode = _node_list_array[ list_level ]._next;
				_list_remove_node( bnode );
				return bnode;
			}
			return nullptr;
		}

		inline void free_node( BuddyNode *bnode )
		{
			_list_remove_node( bnode );
			bnode->_area_size = bnode->_area_start = 0;
			_list_insert_tail( &_free_node_list, bnode );
		}

	private:
		BuddyUsedPageRecord * _allocate_page_record();

		inline void _free_page_record( BuddyUsedPageRecord *record )
		{
			record->next->prev = record->prev;
			record->prev->next = record->next;
			record->next = record->prev = nullptr;
			record->page = nullptr;
		}

		inline void _list_insert_tail( BuddyNode *list, BuddyNode *bnode )
		{
			bnode->_next = list;
			bnode->_prev = list->_prev;
			list->_prev->_next = bnode;
			list->_prev = bnode;
		}

		inline void _list_remove_node( BuddyNode *bnode )
		{
			if ( bnode->_prev )
				bnode->_prev->_next = bnode->_next;
			if ( bnode->_next )
				bnode->_next->_prev = bnode->_prev;
			bnode->_next = bnode->_prev = nullptr;
		}

		inline bool _list_is_empty( BuddyNode *list )
		{
			return ( list->_next == list );
		}
	};

} // namespace mm
