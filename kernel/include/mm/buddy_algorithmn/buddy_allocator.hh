//
// Created by Li Shuang ( pseudonym ) on 2024-04-28
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include <mem/page.hh>
#include <smp/spin_lock.hh>

#include "klib/common.hh"
#include "mm/virtual_page_allocator.hh"

namespace mm
{
	constexpr int _buddy_order_level_cnt = 33;

	extern ulong heap_start;
	extern ulong heap_size;

	constexpr u64 buddy_node_magic =
		0xbd05'4a0e'f9f5'a2deUL; // Tea hash of buddy_node_magic
	constexpr u64 buddy_unused_magic =
		0x9edc'5824'b97d'47b4UL; // Tea hash of buddy_unused_magic

	struct BuddyInfo
	{
		u8 order	 : 6;
		u8 area_flag : 1; // whether area ptr or page ptr?
		u8 in_use	 : 1; // whether allocated or not?
	} __attribute__( ( __packed__ ) );
	static_assert( sizeof( BuddyInfo ) == 1 );

	struct BuddyNode
	{
		union
		{
			u64	  magic;
			void *addr;
		} _head;
		u64		   _area_size;
		BuddyNode *_prev;
		BuddyNode *_next;
	} __attribute__( ( __packed__ ) );
	static_assert( sizeof( BuddyNode ) == 32 );

	class BuddyAllocator : public VirtualPageAllocator
	{
	private:

		using BuddyDesc = BuddyNode;
		hsai::SpinLock _lock;

		void  *_mem_addr = nullptr;
		uint64 _mem_size = 0;

		BuddyNode _buddy_nodes[_buddy_order_level_cnt];
		long	  _buddy_max_order = 0;

		BuddyDesc _area_desc[_buddy_order_level_cnt];
		long	  _area_desc_len = 0;

		BuddyInfo *_alloc_infos = nullptr;
		long	   _infos_size	= 0; // bytes

		ulong _alloc_size = 0;

	public:

		BuddyAllocator() = default;
		BuddyAllocator( void *start, ulong size );

		virtual void *alloc_pages( ulong count ) override;

		virtual int free_pages( void *ptr ) override;

		virtual MemDesc mem_desc() override
		{
			return std::make_tuple( _alloc_size, _mem_size - _alloc_size );
		}

	private:

		void _insert_node( BuddyNode *node, int order )
		{
			node->_next						 = _buddy_nodes[order]._next;
			node->_prev						 = &_buddy_nodes[order];
			_buddy_nodes[order]._next->_prev = node;
			_buddy_nodes[order]._next		 = node;
		}
		void _remove_node( BuddyNode *node )
		{
			node->_prev->_next = node->_next;
			node->_next->_prev = node->_prev;
			node->_next = node->_prev = node;
		}
		bool _has_nodes( int order )
		{
			return _buddy_nodes[order]._next != &_buddy_nodes[order];
		}
		long _count_nodes( int order )
		{
			BuddyNode *pn  = _buddy_nodes[order]._next;
			long	   cnt = 0;
			for ( ; pn != &_buddy_nodes[order]; pn = pn->_next, cnt++ );
			return cnt;
		}
		BuddyDesc *_belong_area( BuddyNode *node )
		{
			ulong addr = (ulong) node;
			for ( int i = 0; i < _area_desc_len; i++ )
			{
				BuddyDesc &bd = _area_desc[i];
				if ( addr < (ulong) bd._head.addr ) continue;
				if ( addr >= (ulong) bd._head.addr + bd._area_size ) continue;
				return &bd;
			}
			return nullptr;
		}
		long _get_info_index( BuddyNode *node )
		{
			ulong addr = (ulong) node;
			return ( addr - (ulong) _mem_addr ) / hsai::page_size;
		}
		void _record_info( BuddyNode *node, u8 order, u8 use, u8 is_area )
		{
			BuddyInfo &info = _alloc_infos[_get_info_index( node )];
			info.in_use		= use;
			info.order		= order;
			info.area_flag	= is_area;
		}
		bool _is_in_use( BuddyNode *node )
		{
			return _alloc_infos[_get_info_index( node )].in_use == 1;
		}

		BuddyNode *_pop_node( int order )
		{
			BuddyNode *node = _buddy_nodes[order]._next;
			_remove_node( node );
			return node;
		}
		BuddyNode *_get_buddy_node( BuddyDesc *bd, BuddyNode *node )
		{
			ulong addr	= (ulong) node - (ulong) bd->_head.addr;
			addr	   ^= node->_area_size;
			addr	   += (ulong) bd->_head.addr;
			return (BuddyNode *) addr;
		}

		// split nd into two
		void _split_node( BuddyNode &nd, BuddyNode &nd1 )
		{
			ulong to_sz	  = nd._area_size >> 1;
			nd._head	  = { .magic = buddy_node_magic };
			nd1._head	  = { .magic = buddy_node_magic };
			nd._area_size = nd1._area_size = to_sz;
		}

		// split node nd into a node in list and nd self
		int _split_node( int order, BuddyNode &nd )
		{
			if ( order <= 0 ) return -1;
			if ( nd._area_size != hsai::page_size << order ) return -2;
			BuddyNode &inlist =
				*( (BuddyNode *) ( (ulong) &nd + ( nd._area_size >> 1 ) ) );

			_split_node( nd, inlist );
			_insert_node( &inlist, order - 1 );
			_record_info( &inlist, order - 1, 0, 1 );

			return 0;
		}

		// combine nd1 into nd, assuming nd and nd1 are buddy
		// nd is the main node, whose address value is smaller
		int _combine_node( BuddyNode &nd, BuddyNode &nd1 )
		{
			if ( nd._area_size != nd1._area_size ) return -1;
			ulong to_sz = nd._area_size << 1;
			nd1			= {
						._head		= { .magic = buddy_unused_magic },
						._area_size = 0,
			};
			nd = { ._head	   = { .magic = buddy_node_magic },
				   ._area_size = to_sz };
			return 0;
		}

	public:

		void debug_print();
		void debug_print_list( int order );
		void debug_ptr_disp();
		void debug_off_disp();
	};


} // namespace mm
