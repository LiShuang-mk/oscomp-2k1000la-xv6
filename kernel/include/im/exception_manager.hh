//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "klib/function.hh"

namespace im
{
	constexpr uint max_ecode_num = 0x40;
	class ExceptionManager
	{
	private:
		smp::Lock _lock;
		std::function<void( void )> _exception_handlers[ max_ecode_num ];

	public:
		ExceptionManager() = default;
		void init( const char *lock_name );
		void kernel_trap();
		void machine_trap();

	public:
		void ahci_read_handle();

	// exception handler 
	private:
		static void handle_pif();
	};

	extern ExceptionManager k_em;
} // namespace im
