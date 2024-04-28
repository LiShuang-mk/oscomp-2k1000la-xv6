//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "mm/page.hh"

namespace mm
{
	class PageTable;
	class BuddyAllocator;

	struct BuddyNode
	{
		uint64 _area_start;
		uint64 _area_size;
		BuddyNode *_prev;
		BuddyNode *_next;
	}__attribute__( ( __packed__ ) );

	constexpr uint node_per_page = mm::pg_size / sizeof( BuddyNode );

	class BuddyAllocator
	{
	private:
		smp::Lock _lock;
		PageTable *_page_table;

		BuddyNode _free_node_list;

		BuddyNode *_node_list_array;
		uint _array_length;

	public:
		BuddyAllocator() {};
		void init( const char *lock_name, PageTable *page_table, void *heap_start, uint64 heap_size );

	private:

		BuddyNode *_alloc_node();

		inline void _list_insert_tail( BuddyNode *list,  BuddyNode *bnode )
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
	};
} // namespace mm
