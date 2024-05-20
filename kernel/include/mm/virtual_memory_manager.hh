//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "mm/page_table.hh"
#include "pm/sharemem.hh"

namespace mm
{
	class VirtualMemoryManager
	{
	private:
		smp::Lock _lock;

	public:
		static uint64 kstack_vm_from_gid( uint gid );

	public:
		VirtualMemoryManager() {};
		void init( const char *lock_name );

		/// @brief map va to pa through pt 
		/// @param pt pagetable to use 
		/// @param va virtual address 
		/// @param size mappint size 
		/// @param pa physical address 
		/// @param flags page table entry flags 
		/// @return success if true 
		bool map_pages( PageTable &pt, uint64 va, uint64 size, uint64 pa, flag_t flags );

		uint64 vmalloc( PageTable &pt, uint64 old_sz, uint64 new_sz );

		uint64 vmdealloc( PageTable &pt, uint64 old_sz, uint64 new_sz );

		/// @brief unmap va from pt
		/// @param pt pagetable to use
		/// @param va virtual address
		/// @param npages num of pages to unmap
		/// @param do_free free physical pages?
		void vmunmap( PageTable &pt, uint64 va, uint64 npages, int do_free );

		PageTable vm_create();

		/// @brief allocate shm
		/// @param pt pagetable to use
		/// @param oldshm oldshm lower address
		/// @param newshm newshm lower address
		/// @param sz shmsize
		/// @param phyaddr 
		/// @return newshm if success
		uint64 allocshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint64 sz, void *phyaddr[ pm::MAX_SHM_PGNUM ] );
		
		/// @brief 
		/// @param pt 
		/// @param sz 
		void vmfree( PageTable &pt, uint64 sz );

		int copy_in( PageTable &pt, void *dst, uint64 src_va, uint64 len );

		int copy_str_in( PageTable &pt, void *dst, uint64 src_va, uint64 max );

		/// @brief map shm pages to physical pages, it is similar with map_pages
		/// @param pt pagetable to use
		/// @param oldshm oldshm lower address
		/// @param newshm newshm lower address
		/// @param sz shmsize
		/// @param phyaddr 
		/// @return newshm if success
		uint64 mapshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint sz, void **phyaddr );

		/// @brief deallocate shm , when allocate shm failed
		/// @param pt pagetable to use
		/// @param oldshm oldshm lower address
		/// @param newshm newshm lower address 
		/// @return oldshm if success
		uint64 deallocshm(PageTable &pt, uint64 oldshm, uint64 newshm );
	private:

	};

	extern VirtualMemoryManager k_vmm;
}