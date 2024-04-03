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

	public:
		PageTable() {};
		void set_base( uint64 addr ) { _base_addr = addr; }
		uint64 get_base() { return _base_addr; }

		/// @brief 软件遍历页表，通常，只能由全局页目录调用
		/// @param va virtual address 
		/// @param alloc either alloc physical page or not 
		/// @return PTE in the last level page table 
		Pte walk( uint64 va, bool alloc );
	};

	extern PageTable k_pagetable;
}