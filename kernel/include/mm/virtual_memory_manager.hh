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
		/// @brief unmap va from pt
		/// @param pt pagetable to use
		/// @param va virtual address
		/// @param npages num of pages to unmap
		/// @param do_free free physical pages?
		void vmunmap( PageTable &pt, uint64 va, uint64 npages, int do_free );
		
		/// @brief allocate shm
		/// @param pt pagetable to use
		/// @param oldshm oldshm lower address
		/// @param newshm newshm lower address
		/// @param sz shmsize
		/// @param phyaddr 
		/// @return newshm if success
		uint64 allocshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint64 sz, void *phyaddr[pm::MAX_SHM_PGNUM] );
		/// @brief 释放一部分内存，更新页表
		/// @param pt pagetable to use
		/// @param sz size
		void vmfree(PageTable &pt, uint64 sz);
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
		/// @brief copy from kernel to user
		/// @param pt pagetable to use
		/// @param va dest virtual address 
		/// @param p source address
		/// @param len length
		/// @return 0 if success, -1 if failed
		int copyout( PageTable &pt, uint64 va, const void *p, uint64 len );
		/// @brief mark a PTE invalid for user access
		/// @param pt 
		/// @param va 
		void uvmclear( PageTable &pt, uint64 va );
		/// @brief allocate memory to grow process from oldsz to newsz
		/// @param pt pagetable to use 
		/// @param oldsz old size
		/// @param newsz new size
		/// @return 
		uint64 uvmalloc( PageTable &pt, uint64 oldsz, uint64 newsz );
		/// @brief deallocate memory to shrink process from oldsz to newsz
		/// @param pt pagetable to use
		/// @param oldsz old size
		/// @param newsz new size
		/// @return 
		uint64 uvmdealloc( PageTable &pt, uint64 oldsz, uint64 newsz );
	private:

	};

	extern VirtualMemoryManager k_vmm;
}