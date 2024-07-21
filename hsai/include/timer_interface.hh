//
// Created by Li Shuang ( pseudonym ) on 2024-07-12 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

namespace hsai
{
	extern __hsai_kernel int handle_tick_intr();

	extern __hsai_kernel ulong get_ticks();


	
	extern __hsai_hal ulong get_main_frequence();

	extern __hsai_hal ulong cycles_per_tick();

	extern __hsai_hal ulong get_hw_time_stamp();

	extern __hsai_hal ulong time_stamp_to_usec( ulong ts );

	extern __hsai_hal ulong usec_to_time_stamp( ulong us );

} // namespace hsai
