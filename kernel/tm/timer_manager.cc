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
#include "klib/klib.hh"

namespace tmm
{
	TimerManager k_tm;

	void TimerManager::init( const char *lock_name )
	{
		_lock.init( lock_name );
		uint64 tcfg_data =
			( ( ( uint64 ) div_fre ) << loongarch::csr::Tcfg::tcfg_initval_s ) |
			( loongarch::csr::Tcfg::tcfg_en_m ) |
			( loongarch::csr::Tcfg::tcfg_periodic_m );
		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, tcfg_data );
		// asm volatile( "csrwr %0, 0x41" : : "r" ( tcfg_data ) );

		_ticks = 0;
	}

	int TimerManager::handle_clock_intr()
	{
		_lock.acquire();
		_ticks++;
		/// TODO: wakeup(&ticks)
		_lock.release();
		return 2;
	}

	timeval TimerManager::get_time_val()
	{
		// uint64 t_val = loongarch::Cpu::read_csr(
		// 	loongarch::csr::tval
		// );
		timeval tv;
		// tv.tv_sec = t_val / qemu_fre;
		// tv.tv_usec = t_val % qemu_fre;
		// tv.tv_usec = qemu_fre_cal_usec( tv.tv_usec );

		tv.tv_sec = _ticks * ms_per_tick / 1000;
		tv.tv_usec = ( ( _ticks * ms_per_tick ) % 1000 ) * 1000;

		log_info( "invoke get time = %d : %d", tv.tv_sec, tv.tv_usec );
		return tv;
	}
} // namespace tm
