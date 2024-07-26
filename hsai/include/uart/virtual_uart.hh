//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "char_device.hh"

namespace hsai
{
	class VirtualUartController __hsai_hal : public CharDevice
	{
	public:
		virtual void init() = 0;
		virtual bool support_stream() override { return false; }
	};

	void register_debug_uart( VirtualUartController * uart_port );

} // namespace hsai
