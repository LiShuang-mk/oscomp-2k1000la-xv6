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
	enum PageEnum : uint64
	{
		pg_size_shift = 12,
		pg_size = 0x1 << pg_size_shift, 		// 4KiB page 
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