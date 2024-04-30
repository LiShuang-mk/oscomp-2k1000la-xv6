//
// Created by Li Shuang ( pseudonym ) on 2024-04-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/buddy.hh"
#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"

namespace mm
{
	void BuddyNodeManager::init( const char *lock_name, uint node_cnt )
	{
		_lock.init( lock_name );

		// init node manage space 

		_used_pages_list_head.page = nullptr;
		_used_pages_list_head.next = _used_pages_list_head.prev = &_used_pages_list_head;
		for ( BuddyUsedPageRecord &up : _used_pages_records )
			up.page = up.next = up.prev = nullptr;

		// init free nodes list 

		_free_node_list._area_size = 0;
		_free_node_list._area_start = 0;
		_free_node_list._next = &_free_node_list;
		_free_node_list._prev = &_free_node_list;

		uint used_page_cnt = ( node_cnt + node_per_page - 1 ) / node_per_page;

		BuddyNode *bnode;
		// full page with buddy-node
		for ( uint npi = 0; npi < used_page_cnt - 1; npi++ )
		{
			bnode = ( BuddyNode * ) mm::k_pmm.alloc_page();
			assert( bnode != nullptr, "buddy-node : no memory to storage buddy node." );
			BuddyUsedPageRecord *record = _allocate_page_record();
			record->page = ( void* ) bnode;
			for ( uint i = 0; i < node_per_page; i++, bnode++ )
			{
				bnode->_area_size = bnode->_area_start = 0;
				// insert bnode into tail 
				_list_insert_tail( &_free_node_list, bnode );
			}
		}
		// last page with buddy-node 
		bnode = ( BuddyNode * ) mm::k_pmm.alloc_page();
		BuddyUsedPageRecord *record = _allocate_page_record();
		record->page = ( void* ) bnode;
		uint rest_node_cnt = node_cnt % node_per_page;
		for ( uint i = 0; i < rest_node_cnt; i++, bnode++ )
		{
			bnode->_area_size = bnode->_area_start = 0;
			// insert bnode into tail 
			_list_insert_tail( &_free_node_list, bnode );
		}

		// init available area list 

		for ( BuddyNode &node : _node_list_array )
		{
			node._area_size = node._area_start = 0;
			node._next = node._prev = &node;
		}
	}

	BuddyNode * BuddyNodeManager::allocate_node()
	{
		if ( _list_is_empty( &_free_node_list ) )
		{
			log_warn( "buddy-allocator : no more nodes to use. return null pointer." );
			return nullptr;
		}

		BuddyNode *bnode = _free_node_list._next;
		_list_remove_node( bnode );
		return bnode;
	}

	void BuddyNodeManager::insert_available_list( BuddyNode * node, uint list_level )
	{
		if ( list_level >= _node_list_array_max_length )
		{
			log_warn(
				"list level too big! max is %d but input %d",
				_node_list_array_max_length, list_level
			);
			return;
		}

		_list_insert_tail( &_node_list_array[ list_level ], node );
	}

// -------- private helper function --------

	BuddyUsedPageRecord * BuddyNodeManager::_allocate_page_record()
	{
		for ( BuddyUsedPageRecord &record : _used_pages_records )
		{
			if ( record.next == nullptr )
			{
				record.prev = &_used_pages_list_head;
				record.next = _used_pages_list_head.next;
				_used_pages_list_head.next->prev = &record;
				_used_pages_list_head.next = &record;
				return &record;
			}
		}
		log_panic( "buddy-alloc : no page record to use" );
		return nullptr;
	}

} // namespace mm
