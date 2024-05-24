//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "tm/timer_manager.hh"
#include "pm/process_manager.hh"
#include "hal/cpu.hh"
#include "hal/csr.hh"
#include "klib/klib.hh"

namespace tmm
{
	TimerManager k_tm;

	void TimerManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		// 自动循环
		_tcfg_data =
			( ( ( uint64 ) div_fre ) << loongarch::csr::Tcfg::tcfg_initval_s ) |
			( loongarch::csr::Tcfg::tcfg_en_m ) |
			( loongarch::csr::Tcfg::tcfg_periodic_m );

		// 非自动循环
		// _tcfg_data =
		// 	( ( ( uint64 ) div_fre ) << loongarch::csr::Tcfg::tcfg_initval_s ) |
		// 	( loongarch::csr::Tcfg::tcfg_en_m );

		loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, _tcfg_data );
		// asm volatile( "csrwr %0, 0x41" : : "r" ( tcfg_data ) );

		_ticks = 0;
	}

	int TimerManager::handle_clock_intr()
	{
		_lock.acquire();
		_ticks++;
		// loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, _tcfg_data );
		/// TODO: wakeup(&ticks)
		pm::k_pm.wakeup( &_ticks );
		_lock.release();
		return 2;
	}

	timeval TimerManager::get_time_val()
	{
		uint64 t_val;
		uint64 cycles_per_tick = ( div_fre << 2 );

		_lock.acquire();
		t_val = cycles_per_tick - loongarch::Cpu::read_csr( loongarch::csr::tval );
		t_val += _ticks * cycles_per_tick;
		_lock.release();

		timeval tv;
		tv.tv_sec = t_val / qemu_fre;
		tv.tv_usec = t_val % qemu_fre;
		tv.tv_usec = qemu_fre_cal_usec( tv.tv_usec );

		// tv.tv_sec = _ticks * ms_per_tick / 1000;
		// tv.tv_usec = ( ( _ticks * ms_per_tick ) % 1000 ) * 1000;

		log_info( "invoke get time = %d : %d", tv.tv_sec, tv.tv_usec );
		return tv;
	}

	int TimerManager::sleep_n_ticks( int n )
	{
		if ( n < 0 ) return -1;

		uint64 tick_tmp;
		pm::Pcb * p = pm::k_pm.get_cur_pcb();

		_lock.acquire();
		tick_tmp = _ticks;
		while ( _ticks - tick_tmp < ( uint64 ) n )
		{
			if ( p->is_killed() )
			{
				_lock.release();
				return -2;
			}
			pm::k_pm.sleep( &_ticks, &_lock );
		}
		_lock.release();

		return 0;
	}

	int TimerManager::sleep_from_tv( timeval tv )
	{
		uint64 n = tv.tv_sec * qemu_fre;
		uint64 cycles_per_tick = ( div_fre << 2 );
		n += qemu_fre_cal_cycles( tv.tv_usec );
		n /= cycles_per_tick;

		return sleep_n_ticks( n );
	}

} // namespace tm
