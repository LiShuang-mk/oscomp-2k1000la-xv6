//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>


namespace im
{
	class InterruptManager
	{
	private:
		hsai::SpinLock _lock;

	public:
		InterruptManager() = default;
		void init( const char *lock_name );

		int handle_dev_intr();

		void clear_uart0_intr();
	};

	extern InterruptManager k_im;
} // namespace im
