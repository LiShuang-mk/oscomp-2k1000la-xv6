//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "uart/virtual_uart.hh"

namespace hsai
{
	VirtualUartController * k_debug_uart = nullptr;
	void register_debug_uart( VirtualUartController * uart_port )
	{
		k_debug_uart = uart_port;
	}
} // namespace hsai
