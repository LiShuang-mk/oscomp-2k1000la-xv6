//
// Created by Li Shuang ( pseudonym ) on 2024-07-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/buddy_algorithmn/buddy_manager.hh"
#include "klib/common.hh"

namespace mm
{
	BuddyManager::BuddyManager( ulong base_addr, BuddyNode *node_list, uint list_length )
		: BuddyManager()
	{
		_lock.init( "buddy manager" );
		_buddy_start = ( void* ) base_addr;
		_node_list = node_list;
		_node_list_length = list_length;

		// clear all node

		for ( uint i = 0; i < _node_list_length; ++i )
		{
			_node_list[ i ] =
			{
				._area_start = 0,
				._area_size = 0
			};
		}

		// init the first node ( the whole area )

		_node_list[ _node_list_length - 1 ] =
		{
			._area_start = _buddy_start,
			._area_size = hsai::page_size << ( _node_list_length - 1 )
		};
	}

	BuddyNode BuddyManager::allocate_node( uint order )
	{
		BuddyNode res = { 0, 0 };

		if ( order >= _node_list_length )
		{
			log_warn( "buddy order too big (%d)", order );
			return res;
		}

		int i = ( int ) order;

		_lock.acquire();

		// find the minium node
		for ( ; i < ( int ) _node_list_length; i++ )
			if ( _node_list[ i ]._area_size != 0 )
				break;

		// no available node
		if ( i >= ( int ) _node_list_length )
		{
			_lock.release();
			log_warn( "no available buddy" );
			return res;
		}

		// pop the minium node
		res = _pop_node( i );

		// split node
		for ( ; i > ( int ) order; i-- )
			if ( [[maybe_unused]] int ret = _split_node( i, res ) < 0 )
				assert( ret >= 0, "split buddy fail" );

		_lock.release();

		return res;
	}

	void BuddyManager::recycle_node( uint order, BuddyNode nd )
	{
		assert( hsai::page_size << order == nd._area_size, "" );

		int i = ( int ) order;

		_lock.acquire();

		for ( ; _node_list[ i ]._area_size != 0; i++ )
		{
			int ret = _combine_node( i, nd, nd );
			assert( ret >= 0, "" );
		}

		_node_list[ i ] = nd;

		_lock.release();
	}

	void BuddyManager::_debug_print_node()
	{
		if ( _node_list_length )
		{
			printf( "\t" );
			for ( uint i = 0; i < _node_list_length; i++ )
			{
				printf( "-->%d", _node_list[ i ]._area_size != 0 );
			}
			printf( "\n" );
		}
	}

	bool BuddyManager::has_node( BuddyNode node )
	{
		_lock.acquire();
		bool res = ( ( node._area_start >= _buddy_start )
			&& ( ( ulong ) node._area_start + node._area_size <= ( ulong ) _buddy_start + ( hsai::page_size << ( _node_list_length - 1 ) ) ) );
		_lock.release();
		return res;
	}

} // namespace mm
