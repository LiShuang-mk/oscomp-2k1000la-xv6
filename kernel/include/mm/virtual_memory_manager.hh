//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "mm/page_table.hh"
#include "pm/sharemem.hh"

#include <smp/spin_lock.hh>

#include <EASTL/string.h>

namespace mm
{
	class VirtualMemoryManager
	{
	private:
		hsai::SpinLock _lock;

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

		uint64 vm_alloc( PageTable &pt, uint64 old_sz, uint64 new_sz, bool executable = false, bool for_user = true );

		uint64 vm_dealloc( PageTable &pt, uint64 old_sz, uint64 new_sz );

		/// @brief unmap va from pt
		/// @param pt pagetable to use
		/// @param va virtual address
		/// @param npages num of pages to unmap
		/// @param do_free free physical pages?
		void vm_unmap( PageTable &pt, uint64 va, uint64 npages, int do_free );

		PageTable vm_create();

		int vm_copy( PageTable &old_pt, PageTable &new_pt, uint64 start, uint64 size );

		/// @brief allocate shm
		/// @param pt pagetable to use
		/// @param oldshm oldshm lower address
		/// @param newshm newshm lower address
		/// @param sz shmsize
		/// @param phyaddr 
		/// @return newshm if success
		uint64 allocshm( PageTable &pt, uint64 oldshm, uint64 newshm, uint64 sz, void *phyaddr[ pm::MAX_SHM_PGNUM ] );

		int copy_in( PageTable &pt, void *dst, uint64 src_va, uint64 len );

		int copy_str_in( PageTable &pt, void *dst, uint64 src_va, uint64 max );
		int copy_str_in( PageTable &pt, eastl::string &dst, uint64 src_va, uint64 max );

		int either_copy_in( void *dst, bool user_src, uint64 src, uint64 len );

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
		uint64 deallocshm( PageTable &pt, uint64 oldshm, uint64 newshm );

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
		int vm_set_super( PageTable &pt, uint64 va, ulong page_cnt );

		/// @brief 设置 va 起始的用户空间每个字节的值
		/// @param pt 用户页表
		/// @param va 虚拟地址，需要页对齐
		/// @param n 需要设置的值
		/// @param page_cnt 需要设置的页面数
		/// @return 失败返回负数
		int uvmset( PageTable &pt, void * va, u8 n, int page_cnt );

	public:
		bool map_code_pages( PageTable &pt, ulong va, ulong size, ulong pa, bool for_user )
		{
			return map_pages( pt, va, size, pa,
				hsai::Pte::map_code_page_flags() | hsai::Pte::valid_flag()
				| ( for_user ? ( hsai::Pte::user_plv_flag() ) : ( hsai::Pte::super_plv_flag() ) )
			);
		};
		bool map_data_pages( PageTable &pt, ulong va, ulong size, ulong pa, bool for_user )
		{
			return map_pages( pt, va, size, pa,
				hsai::Pte::map_data_page_flags() | hsai::Pte::valid_flag()
				| ( for_user ? ( hsai::Pte::user_plv_flag() ) : ( hsai::Pte::super_plv_flag() ) )
			);
		};

	};

	extern VirtualMemoryManager k_vmm;
}