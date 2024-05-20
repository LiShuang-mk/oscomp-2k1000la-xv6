//
// Created by Li shuang ( pseudonym ) on 2024-03-29 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "mm/pte.hh"

namespace mm
{
	class PageTable
	{
	private:
		uint64 _base_addr;
		bool _is_global = false;

	public:
		PageTable();
		void set_base( uint64 addr ) { _base_addr = addr; }
		uint64 get_base() { return _base_addr; }
		void set_global() { _is_global = true; }
		void unset_global() { _is_global = false; }
		bool is_null() { return _base_addr == 0; }
		
		/// @brief 软件遍历页表，通常，只能由全局页目录调用
		/// @param va virtual address 
		/// @param alloc either alloc physical page or not 
		/// @return PTE in the last level page table 
		Pte walk( uint64 va, bool alloc );

		/// @brief 软件遍历页表，通常只能由用户的全局页目录调用
		/// @param va virtual address
		/// @return physical address mapped from va
		void * walk_addr( uint64 va );

		/// @brief 递归地释放页表中的所有页面
		void freewalk();

		uint64 dir3_num( uint64 va );
		uint64 dir2_num( uint64 va );
		uint64 dir1_num( uint64 va );
		uint64 pt_num( uint64 va );

		uint64 get_pte_data( uint64 index ) { return ( uint64 ) ( ( pte_t * ) _base_addr )[ index ]; }
		void   reset_pte_data(uint64 index)   { ((pte_t *) _base_addr)[ index ] = 0; }
		uint64 get_pte_addr( uint64 index ) { return ( uint64 ) & ( ( pte_t * ) _base_addr )[ index ]; }
		Pte get_pte( uint64 index ) { return Pte( &( ( pte_t * ) _base_addr )[ index ] ); }


	private:
		bool _walk_to_next_level( Pte pte, bool alloc, PageTable &pt );
	};

	extern PageTable k_pagetable;
}