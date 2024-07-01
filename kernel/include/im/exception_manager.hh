//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "klib/function.hh"

#include <smp/spin_lock.hh>

namespace im
{
	constexpr uint max_ecode_num = 0x40;
	extern char _user_or_kernel;
	class ExceptionManager
	{
	private:
		hsai::SpinLock _lock;
		std::function<void( uint32 )> _exception_handlers[ max_ecode_num ];


	public:
		ExceptionManager() = default;
		void init( const char *lock_name );
		void kernel_trap();
		void user_trap( uint64 estat );
		void user_trap_ret();
		void machine_trap();
		int dev_intr();

	public:
		void ahci_read_handle();

	// exception handler 
	private:
		// static void handle_pif();
		void _init_exception_handler();

		void _syscall();
	};

	extern ExceptionManager k_em;
} // namespace im
