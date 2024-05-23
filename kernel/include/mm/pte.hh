//
// Created by Li shuang ( pseudonym ) on 2024-04-02 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"

#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
#include "mm/page.hh"

namespace mm
{
	class PageTable;

	/// @brief page table entry 
	class Pte
	{
		friend PageTable;
	private:
		pte_t *_data_addr = 0;
	public:
		Pte() = default;
		Pte( pte_t *addr ) : _data_addr( addr ) {};
		void set_addr( pte_t *addr ) { _data_addr = addr; }
		bool is_null() { return _data_addr == 0; }

		bool is_valid() { return ( ( *_data_addr & loongarch::PteEnum::valid_m ) != 0 ); }
		bool is_dirty() { return ( ( *_data_addr & loongarch::PteEnum::dirty_m ) != 0 ); }
		uint64 plv() { return ( ( *_data_addr & loongarch::PteEnum::plv_m ) >> loongarch::PteEnum::plv_s ); }
		uint64 mat() { return ( ( *_data_addr & loongarch::PteEnum::mat_m ) >> loongarch::PteEnum::mat_s ); }
		uint64 flags() { return *_data_addr & loongarch::PteEnum::b_flags_m; }
		void set_plv() { *_data_addr |= loongarch::PteEnum::plv_m; }
		void unset_plv() { *_data_addr &= ~loongarch::PteEnum::plv_m; }
		bool is_global() { return ( ( *_data_addr & loongarch::PteEnum::b_global_m ) != 0 ); }
		bool is_present() { return ( ( *_data_addr & loongarch::PteEnum::presence_m ) != 0 ); }
		bool is_writable() { return ( ( *_data_addr & loongarch::PteEnum::writable_m ) != 0 ); }
		void *pa() { return ( void* ) ( ( *_data_addr & loongarch::PteEnum::b_pa_m ) | loongarch::qemuls2k::dmwin::win_0 ); }
		bool is_readable() { return ( ( *_data_addr & loongarch::PteEnum::nr_m ) == 0 ); }
		bool is_executable() { return ( ( *_data_addr & loongarch::PteEnum::nx_m ) == 0 ); }
		bool is_restrict_plv() { return ( ( *_data_addr & loongarch::PteEnum::rplv_m ) != 0 ); }
		bool is_leaf() { return ( ( *_data_addr & loongarch::qemuls2k::ls2kPgEnum::pg_flags_mask ) != 1 ); }

		void set_data( uint64 data ) { *_data_addr |= data; }

		// 慎用！！！这个函数会使PTE的值清零！
		void clear_data() { *_data_addr = 0; }
		uint64 get_data() { return *_data_addr; }

	};
}