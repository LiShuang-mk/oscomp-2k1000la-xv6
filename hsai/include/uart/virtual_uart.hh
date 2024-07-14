//
// Created by Li Shuang ( pseudonym ) on 2024-06-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

namespace hsai
{
	class VirtualUartController __hsai_hal
	{
	public:
		virtual void init() = 0;
		virtual void put_char_sync( int c ) = 0;
		virtual void put_char( int c ) = 0;
		// virtual int get_char( int c ) = 0;
		virtual void handle_interrupt() = 0;
	};

	extern VirtualUartController * k_debug_uart;
	void register_debug_uart( VirtualUartController * uart_port );

} // namespace hsai
