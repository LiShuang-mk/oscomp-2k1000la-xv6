//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "uart/virtual_uart.hh"

#include "device_manager.hh"
#include "hsai_global.hh"

namespace hsai
{
	void register_debug_uart( VirtualUartController * uart_port )
	{
		k_devm.register_char_device( ( CharDevice * ) uart_port, "console" );
	}
} // namespace hsai
