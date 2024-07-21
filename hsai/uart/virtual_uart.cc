//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "uart/virtual_uart.hh"
#include "hsai_defs.h"
#include "device_manager.hh"
#include "hsai_global.hh"


namespace hsai
{
	void register_debug_uart( VirtualUartController * uart_port )
	{
		k_devm.register_char_device( ( CharDevice * ) uart_port, DEFAULT_DEBUG_CONSOLE_NAME );
	}
} // namespace hsai
