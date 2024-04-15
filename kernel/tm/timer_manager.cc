//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "tm/timer_manager.hh"
#include "hal/cpu.hh"
#include "hal/csr.hh"

namespace tm
{
	TimerManager k_tm;

	void TimerManager::init( const char *lock_name )
	{
		_lock.init( lock_name );
		uint64 tcfg_data =
			( ( ( uint64 ) div_fre ) << loongarch::csr::tcfg::tcfg_initval_s ) |
			( loongarch::csr::tcfg::tcfg_en_m ) |
			( loongarch::csr::tcfg::tcfg_periodic_m );
		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, tcfg_data );
	}
} // namespace tm
