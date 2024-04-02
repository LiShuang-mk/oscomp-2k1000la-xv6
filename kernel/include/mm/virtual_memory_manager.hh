//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/physical_memory_manager.hh"
#include "mm/page_table.hh"

namespace mm
{
	class VirtualMemoryManager
	{
	private:


	public:
		static uint64 kstack_vm_from_gid( uint gid );

	public:
		VirtualMemoryManager() {};
		void init();

		/// @brief map va to pa through pt 
		/// @param pt pagetable to use 
		/// @param va virtual address 
		/// @param size mappint size 
		/// @param pa physical address 
		/// @param flags page table entry flags 
		/// @return success if true 
		bool map_pages( const PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags );

		/// @brief 软件遍历页表
		/// @param pt pagetalbe 
		/// @param va virtual address 
		/// @param alloc either alloc physical page or not 
		/// @return 
		pte_t *walk( const PageTable &pt, uint64 va, bool alloc );

	private:
		bool _walk_to_next_level( PageTable &pt, pte_t *pte, bool alloc );
	};

	extern VirtualMemoryManager k_vmm;
}