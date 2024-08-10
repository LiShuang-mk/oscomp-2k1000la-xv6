//
// Created by Li Shuang ( pseudonym ) on 2024-04-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "mm/buddy_algorithmn/buddy_allocator.hh"
#include "klib/klib.hh"

namespace mm
{
	BuddyAllocator::BuddyAllocator( void *mem, ulong size )
	{
		assert( ( ( uint64 ) mem % hsai::page_size ) == 0, "[buddy] 起始地址不与页大小对齐 (%p)", mem );
		assert( size >= 64 * hsai::page_size, "[buddy] 用于伙伴算法的内存太小! (%d bytes)", size );
		assert( ( size & ( hsai::page_size - 1 ) ) == 0, "[buddy] 用于伙伴算法的大小不是页对齐的 (%xH)", size );

		_lock.init( "buddy" );

		// 计算 infos 长度，页对齐

		_infos_size = ( long ) size / ( hsai::page_size / sizeof( BuddyInfo ) );	// each page has one buddy-info
		_infos_size = hsai::page_round_up( ( ulong ) _infos_size );

		// mem 末尾存储 infos

		_alloc_infos = ( BuddyInfo * ) ( ( ulong ) mem + size - ( ulong ) _infos_size );
		memset( _alloc_infos, 0, _infos_size );

		_mem_addr = mem;
		_mem_size = size - _infos_size;
		for ( BuddyNode &node : _buddy_nodes )			// reset nodes 
		{
			node = {
				._head = {.magic = buddy_unused_magic },
				._area_size = 0,
				._prev = &node,
				._next = &node
			};
		}

		ulong pg_cnt = _mem_size / hsai::page_size;					// page counts
		ulong area_start = ( ulong ) _mem_addr;			// the start address of each area

		for ( ulong l = 1, k = 0; l != 0; l <<= 1, k++ )
		{
			if ( l & pg_cnt )
			{
				BuddyNode &node = *( BuddyNode * ) area_start;		// each area has one init node in the front
				node = {
					._head = {.magic = buddy_node_magic},
					._area_size = hsai::page_size << k
				};
				_insert_node( &node, ( int ) k );					// record the node as available node
				_record_info( &node, k, 0, 1 );
				_buddy_max_order = k;

				_area_desc[ _area_desc_len ] = {					// create an area descriptor
					._head = {.addr = ( void * ) area_start},
					._area_size = hsai::page_size << k,
				};
				_area_desc_len++;

				area_start += node._area_size;						// move area_start to the next area
			}
		}
	}

	void *BuddyAllocator::alloc_pages( ulong count )
	{
		if ( count == 0 )
		{
			printf( "buddy-alloc : request ZERO pages. return null pointer." );
			return nullptr;
		}

		// get the order of the node including 'count' pages
		// 2^(order-1) < count <= 2^(order)
		int order = 0;
		for ( ;; order++ )
			if ( ( 1U << order ) >= count )
				break;

		_lock.acquire();

		int i = order;

		// find the minium node
		for ( ; i <= ( int ) _buddy_max_order; i++ )
			if ( _has_nodes( i ) )
				break;

		// no available node
		if ( i > ( int ) _buddy_max_order )
		{
			// _lock.release();
			// log_warn( "no available buddy" );
			printf( "[warn] no available buddy\n" );
			return nullptr;
		}

		// pop the minium node
		BuddyNode *node = _pop_node( i );
		if ( node->_head.magic != buddy_node_magic )
		{
			// log_warn( "find buddy-node but isn't inited for magic 0x%x != 0x%x", node->magic, buddy_node_magic );
			printf( "[warn] find buddy-node but isn't inited for magic(0x%x) != 0x%x", node->_head.magic, buddy_node_magic );
			return nullptr;
		}

		// split node
		for ( ; i > ( int ) order; i-- )
		{
			if ( [[maybe_unused]] int ret = _split_node( i, *node ) < 0 )
			{
				// assert( ret >= 0, "split buddy fail" );
				assert( ret >= 0, "[buddy] split node fail" );
			}
		}

		_record_info( node, order, 1, 1 );

		_alloc_size += ( 1UL << order ) * hsai::page_size;

		_lock.release();

		return ( void * ) node;
	}

	int BuddyAllocator::free_pages( void *ptr )
	{
		if ( ptr == nullptr )		// ignore nullptr
			return -1;

		if ( !hsai::is_page_align( ( u64 ) ptr ) )
		{
			log_error( "[buddy] 释放的页指针没有对齐! (%p)", ptr );
			return -2;
		}

		_lock.acquire();

		long info_idx = _get_info_index( ( BuddyNode * ) ptr );
		assert( info_idx < _infos_size, "[buddy] bad buddy-info index" );
		BuddyInfo &info = _alloc_infos[ info_idx ];

		if ( info.area_flag == 0 )					// 这是一个页指针，分配和回收动作对其无效
		{
			_lock.release();
			return 0;
		}
		if ( info.in_use == 0 )
		{
			printf( "repeat free %p", ptr );
			_lock.release();
			return -3;
		}
		assert( info.order <= _buddy_max_order, "[buddy] 释放的页面似乎元数据失效？(%p)", ptr );		// bad order
		info.in_use = 0;

		int order = info.order;
		_alloc_size -= ( 1UL << order ) * hsai::page_size;

		BuddyNode *node = ( BuddyNode * ) ptr;
		BuddyDesc *bd = _belong_area( node );
		*node = {
			._head = {.magic = buddy_node_magic},
			._area_size = hsai::page_size << order,
		};

		for ( ;;)
		{
			// 当前区域最大的节点，无法合并为更大的节点
			if ( node->_area_size == bd->_area_size )
				break;
			// 查看伙伴节点是否空闲
			BuddyNode *buddy = _get_buddy_node( bd, node );
			if ( _is_in_use( buddy ) )		// 伙伴节点正在被使用，无法合并
				break;
			else							// 伙伴节点空闲，考虑合并
			{
				if ( node->_area_size != buddy->_area_size )		// 伙伴节点已经被拆分，无法合并
					break;
				_remove_node( buddy );
				if ( ( ulong ) node > ( ulong ) buddy )				// 地址较小的是主节点
				{	// 交换
					node = ( BuddyNode * ) ( ( ulong ) node ^ ( ulong ) buddy );
					buddy = ( BuddyNode * ) ( ( ulong ) node ^ ( ulong ) buddy );
					node = ( BuddyNode * ) ( ( ulong ) node ^ ( ulong ) buddy );
				}
				_record_info( buddy, 0, 0, 0 );						// 被合并的节点指针记录为页指针
				assert( _combine_node( *node, *buddy ) >= 0, "[buddy] combine node error" );
				order++;
			}
		}

		_insert_node( node, order );
		_record_info( node, order, 0, 1 );

		_lock.release();
		return 0;
	}

#ifndef COLOR_PRINT
#define COLOR_PRINT

#define RED_COLOR_PINRT "\033[31m"
#define GREEN_COLOR_PRINT "\033[32m"
#define BLUE_COLOR_PRINT "\033[34m"
#define CYAN_COLOR_PINRT "\033[36m"
#define CLEAR_COLOR_PRINT "\033[0m"

#endif

	static int debug_cnt = 0;
	static int debug_disp_mod = 0;
	void BuddyAllocator::debug_ptr_disp() { debug_disp_mod = 0; }
	void BuddyAllocator::debug_off_disp() { debug_disp_mod = 1; }

	void BuddyAllocator::debug_print_list( int order )
	{
		printf( GREEN_COLOR_PRINT "\t[%d:%d]" CLEAR_COLOR_PRINT, order, 1 << order );
		BuddyNode *pn = _buddy_nodes[ order ]._next;
		for ( ; pn != &_buddy_nodes[ order ]; pn = pn->_next )
		{
			ulong addr = ( ulong ) pn;
			if ( debug_disp_mod == 1 )
				addr -= ( ulong ) _mem_addr;
			printf( "-->0x%x", addr );
		}
		printf( "\n" );
	}

	void BuddyAllocator::debug_print()
	{
		printf( BLUE_COLOR_PRINT "<No.%d debug>\n" CLEAR_COLOR_PRINT, debug_cnt );
		for ( int i = 0; i <= _buddy_max_order; ++i )
			debug_print_list( i );
		printf( BLUE_COLOR_PRINT "<node cnt>" );
		for ( int i = 0; i <= _buddy_max_order; ++i )
		{
			long cnt = _count_nodes( i );
			printf( "-->%d", cnt );
		}
		printf( CLEAR_COLOR_PRINT "\n" );
		++debug_cnt;
	}

} // namespace mm
