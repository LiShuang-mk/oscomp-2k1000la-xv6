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


namespace mm
{
	/// @brief page table entry 
	class Pte
	{
	private:
		pte_t _data = 0;
	public:
		Pte() = default;
		Pte( uint64 data ) : _data( data ) {};


		bool is_valid() { return ( ( _data & loongarch::PteEnum::valid_m ) != 0 ); }
		bool is_dirty() { return ( ( _data & loongarch::PteEnum::dirty_m ) != 0 ); }
		uint64 plv() { return ( ( _data & loongarch::PteEnum::plv_m ) ); }
		uint64 mat() { return ( ( _data & loongarch::PteEnum::mat_m ) ); }
		bool is_global() { return ( ( _data & loongarch::PteEnum::b_global_m ) != 0 ); }
		bool is_present() { return ( ( _data & loongarch::PteEnum::presence_m ) != 0 ); }
		bool is_writable() { return ( ( _data & loongarch::PteEnum::writable_m ) != 0 ); }
		void *pa() { return ( void* ) ( ( _data & loongarch::PteEnum::b_pa_m ) ); }
		bool is_readable() { return ( ( _data & loongarch::PteEnum::nr_m ) == 0 ); }
		bool is_executable() { return ( ( _data & loongarch::PteEnum::nx_m ) == 0 ); }
		bool is_restrict_plv() { return ( ( _data & loongarch::PteEnum::rplv_m ) != 0 ); }
	};
}