//
// Created by Li Shuang ( pseudonym ) on 2024-07-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

namespace hsai
{
	class VirtualInterruptManager
	{
	public:
		virtual int handle_dev_intr() = 0;

	public:
		static int register_interrupt_manager( VirtualInterruptManager * im );
	};

} // namespace hsai
