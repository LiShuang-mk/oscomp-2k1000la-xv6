//
// Created by Li shuang ( pseudonym ) on 2024-03-28 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "smp/lock.hh"

namespace mm
{
	/// @brief page enum for page info 
	/// @details __________________________________________________
	/// | the paging schema: 
	/// | 	63 ... 48 -- must be zero 
	/// | 	47 ... 39 -- 9 bits of level-3 virtual page number 
	/// | 	38 ... 30 -- 9 bits of level-2 virtual page number 
	/// | 	29 ... 21 -- 9 bits of level-1 virtual page number 
	/// | 	20 ... 12 -- 9 bits of last level virtual page number 
	///	|	11 ... 0  -- 12 bits of byte offset within the page 
	/// | _________________________________________________________
	enum PageEnum : uint64
	{
		pg_size_shift = 12,
		pg_size = 0x1 << pg_size_shift, 		// 4KiB page 
		pg_offset_mask = 0xFFFUL,

		pt_bits_null = 0, 
		pt_bits_width = 9,
		pt_bits_mask = 0x1FFUL,
		pt_vpn_shift = pg_size_shift,
		pt_vpn_mask = pt_bits_mask << pt_vpn_shift,

		dir1_vpn_shift = 21,
		dir2_vpn_shift = 30, 
		dir3_vpn_shift = 39,
		dir4_vpn_shift = 48,

		dir1_vpn_mask = pt_bits_mask << dir1_vpn_shift,
		dir2_vpn_mask = pt_bits_mask << dir2_vpn_shift,
		dir3_vpn_mask = pt_bits_mask << dir3_vpn_shift,

		pte_width = 0x0UL 
	};

	/// @brief This struct just be used to unused physical page;
	/// @details A simple link list connect all unused page;
	struct PageHead
	{
		struct PageHead *_next;
	};

	constexpr uint64 page_round_up( uint64 addr )
	{
		return ( ( addr + pg_size - 1 ) & ~( pg_size - 1 ) );
	}

	constexpr uint64 page_round_down( uint64 addr )
	{
		return ( addr & ~( pg_size - 1 ) );
	}



	// enum PageStatus
	// {
	// 	undefined,		// error status 
	// 	free,			// free in kernel page list, only physical page using 
	// 	not_init, 		// has been alloc but not init 
	// 	used,			// is being used by process 
	// };


	// /// @brief virtual class for page 
	// class Page
	// {
	// public:

	// };
}