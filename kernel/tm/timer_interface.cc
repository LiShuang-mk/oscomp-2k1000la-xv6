//
// Created by Li Shuang ( pseudonym ) on 2024-07-12 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include <timer_interface.hh>

#include "tm/timer_manager.hh"

namespace hsai
{
	int handle_tick_intr()
	{
		return tmm::k_tm.handle_clock_intr();
	}

	ulong get_ticks()
	{
		return tmm::k_tm.get_ticks();
	}
} // namespace hsai
