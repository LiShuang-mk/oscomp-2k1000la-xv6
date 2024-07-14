//
// Created by Li shuang ( pseudonym ) on 2024-03-26 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/uart.hh"

#include <smp/spin_lock.hh>


namespace dev
{
	constexpr uint console_input_buf_size = 128;
	
	class Console
	{
	private:
		hsai::SpinLock _lock;
		bool _use_k_debug_uart;
		const int _backspace = 0x100;
		char _ibuf[ console_input_buf_size ];
		uint _ridx;			// read index
		uint _widx;			// write index
		uint _eidx;			// edit index
	public:
		Console();
		void init( const char *name, bool use_default_uart );
		void putc( char c );

		int write( int from_user, uint64 src, uint64 n );
		int read( int to_user, uint64 dst, uint64 n );

		void handle_uart_intr();
	};

	extern Console k_console;

} // namespace dev
