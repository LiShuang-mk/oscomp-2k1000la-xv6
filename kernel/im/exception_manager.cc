//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "im/exception_manager.hh"
#include "hal/cpu.hh"
#include "klib/common.hh"

extern "C" {
	extern void kernelvec();
	extern void handle_tlbr();
	extern void handle_merr();
}

namespace im
{
	ExceptionManager k_em;

	void ExceptionManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		uint32 ecfg_data =
			( 0x0U << loongarch::csr::ecfg_vs_s ) |
			( loongarch::csr::itr_hwi_m << loongarch::csr::ecfg_lie_s ) |
			( loongarch::csr::itr_ti_m << loongarch::csr::ecfg_lie_s );
		loongarch::Cpu::write_csr( loongarch::csr::ecfg, ecfg_data );
		loongarch::Cpu::write_csr( loongarch::csr::eentry, ( uint64 ) kernelvec );
		loongarch::Cpu::write_csr( loongarch::csr::tlbrentry, ( uint64 ) handle_tlbr );
		loongarch::Cpu::write_csr( loongarch::csr::merrentry, ( uint64 ) handle_merr );

		
		// loongarch::Cpu::interrupt_on();
	}

	void im::ExceptionManager::kernel_trap()
	{
		log_panic( "not implement" );
	}

	void im::ExceptionManager::machine_trap()
	{
		log_panic( "not implement" );
	}
}// namespace im

