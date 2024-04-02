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
	};
}