//
// Created by Li Shuang ( pseudonym ) on 2024-04-30 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <mem/page.hh>
#include <smp/spin_lock.hh>

namespace mm
{
	// The relation between buddy-node and heap area is one to one correspendence.
	// an area may include several pages or more 
	struct BuddyNode
	{
		void * _area_start;
		uint64 _area_size;
	}__attribute__( ( __packed__ ) );

	// buddy 算法最小支持的内存大小，必须是2的幂
	constexpr ulong buddy_mem_min_size = 64 * _1K;
	static_assert( ( buddy_mem_min_size & ( buddy_mem_min_size - 1 ) ) == 0 );
	constexpr ulong buddy_mem_min_size_shift = [] () -> ulong
	{
		ulong l = 1, k = 0;
		for ( ; ( l & buddy_mem_min_size ) == 0; l <<= 1, k++ );
		return k;
	}( );

	// buddy 算法最大可支持的内存大小，必须是最小支持内存大小的2次方倍数
	constexpr ulong buddy_mem_max_size = 1 * _1G;
	constexpr ulong buddy_mem_max_size_shift = [] () -> ulong
	{
		ulong l = 1, k = 0;
		for ( ; ( l & buddy_mem_max_size ) == 0; l <<= 1, k++ );
		return k;
	}( );

	// 最大支持内存分页数
	constexpr ulong buddy_pages_max_cnt = buddy_mem_max_size / hsai::page_size;

	// 以最小支持内存大小为最小粒度
	constexpr ulong buddy_part_size = buddy_mem_min_size;
	constexpr ulong buddy_part_size_shift = buddy_mem_min_size_shift;

	// 单个最小颗粒所需节点数量
	constexpr ulong buddy_node_min_cnt = buddy_mem_min_size_shift - hsai::page_size_shift + 1;

	// 最大支持内存可分为最小颗粒的数量
	constexpr ulong buddy_max_part_cnt = buddy_mem_max_size / buddy_part_size;
	constexpr ulong buddy_max_part_cnt_shift = buddy_mem_max_size_shift - buddy_part_size_shift;
	static_assert( ( buddy_max_part_cnt & ( buddy_max_part_cnt - 1 ) ) == 0 );

	// buddy manager 的最大数量
	constexpr ulong buddy_manager_max_cnt = buddy_max_part_cnt_shift;

	// 所需最大buddy节点数量
	constexpr ulong buddy_node_max_cnt = buddy_node_min_cnt * ( buddy_manager_max_cnt + 1 ) * buddy_manager_max_cnt / 2;

} // namespace mm
