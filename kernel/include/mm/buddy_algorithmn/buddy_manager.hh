//
// Created by Li Shuang ( pseudonym ) on 2024-07-17 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "buddy.hh"

#include <mem/page.hh>
#include <smp/spin_lock.hh>

namespace mm
{
	class BuddyAllocator;

	class BuddyManager
	{
		friend BuddyAllocator;

	private:
		hsai::SpinLock _lock;

		// the buddy area start address
		ulong _buddy_start;

		// available area node array, whose index corresponds to the order
		BuddyNode *_node_list;

		// the length of the available area node array
		uint _node_list_length;

	public:

		BuddyManager() = default;

		BuddyManager( ulong base_addr, BuddyNode *node_list, uint list_length );

		bool has_node( BuddyNode node );

		BuddyNode allocate_node( uint order );

		void recycle_node( uint order, BuddyNode nd );

	private:

		// split nd into nd1 and nd2
		void _split_node( BuddyNode &nd, BuddyNode &nd1, BuddyNode &nd2 )
		{
			BuddyNode tmp = nd;
			nd = { 0, 0 };

			ulong to_sz = tmp._area_size >> 1;
			nd1._area_size = nd2._area_size = to_sz;

			nd1._area_start = tmp._area_start;
			nd2._area_start = nd1._area_start + to_sz;

		}

		// split node nd into a node in list and nd self
		int _split_node( uint order, BuddyNode &nd )
		{
			if ( order == 0 )
				return -1;
			if ( nd._area_size != hsai::page_size << order )
				return -2;
			if ( _node_list[ order - 1 ]._area_size != 0 )
				return -3;

			_split_node( nd, _node_list[ order - 1 ], nd );

			return 0;
		}

		// remove a node and return the node
		BuddyNode _pop_node( uint order )
		{
			BuddyNode tmp = { 0,0 };

			if ( order >= _node_list_length )
				return tmp;

			tmp = _node_list[ order ];
			_node_list[ order ] = { 0,0 };
			return tmp;
		}

		void _combine_node( BuddyNode &nd1, BuddyNode &nd2, BuddyNode &nd )
		{
			if ( nd1._area_size != nd2._area_size )
				return;

			// assume nd1 and nd2 are buddy

			ulong comb_start = ( nd1._area_start < nd2._area_start ) ? nd1._area_start : nd2._area_start;
			ulong comb_size = nd1._area_size << 1;

			nd1 = nd2 = { 0,0 };
			nd = { ._area_start = comb_start, ._area_size = comb_size };
		}

		int _combine_node( uint from_order, BuddyNode &from_nd, BuddyNode &to_nd )
		{
			if ( from_order >= _node_list_length )
				return -1;
			if ( hsai::page_size << from_order != from_nd._area_size )
				return -2;
			if ( _node_list[ from_order ]._area_size == 0 )
				return -3;

			_combine_node( _node_list[ from_order ], from_nd, to_nd );

			return 0;
		}

	public:

		void _debug_print_node();

	};

} // namespace mm
