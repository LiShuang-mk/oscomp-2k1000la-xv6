//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "tm/timer_manager.hh"
#include "pm/process_manager.hh"
#include "klib/klib.hh"

#include <timer_interface.hh>

namespace tmm
{
	TimerManager k_tm;

	void TimerManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		_ticks = 0;

		// close_ti_intr();
	}

	// void TimerManager::open_ti_intr()
	// {
	// 	_lock.acquire();
	// 	_tcfg_data |= ( loongarch::csr::Tcfg::tcfg_en_m );
	// 	loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, _tcfg_data );
	// 	_lock.release();
	// }

	// void TimerManager::close_ti_intr()
	// {
	// 	_lock.acquire();
	// 	_tcfg_data &= ~( loongarch::csr::Tcfg::tcfg_en_m );
	// 	loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, _tcfg_data );
	// 	_lock.release();
	// }

	int TimerManager::handle_clock_intr()
	{
		_lock.acquire();
		_ticks++;
		// printf( "t" );
		// loongarch::Cpu::write_csr( loongarch::csr::CsrAddr::tcfg, _tcfg_data );
		pm::k_pm.wakeup( &_ticks );
		_lock.release();
		return 0;
	}

	timeval TimerManager::get_time_val()
	{
		uint64 t_val;
		uint64 cpt = hsai::cycles_per_tick();

		_lock.acquire();
		t_val = hsai::get_hw_time_stamp();
		t_val += _ticks * cpt;
		_lock.release();

		timeval tv;
		tv.tv_sec = t_val / hsai::get_main_frequence();
		tv.tv_usec = t_val % hsai::get_main_frequence();
		tv.tv_usec = hsai::time_stamp_to_usec( tv.tv_usec );

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
		uint64 n = tv.tv_sec * hsai::get_main_frequence();
		uint64 cpt = hsai::cycles_per_tick();
		n += hsai::usec_to_time_stamp( tv.tv_usec );
		n /= cpt;

		return sleep_n_ticks( n );
	}

	int TimerManager::clock_gettime( SystemClockId cid, timespec * tp )
	{
		if ( tp == nullptr )
			return 0;

		uint64 t_val;
		uint64 cpt = hsai::cycles_per_tick();
		uint64 freq = hsai::get_main_frequence();

		_lock.acquire();
		t_val = hsai::get_hw_time_stamp();
		t_val += _ticks * cpt;
		_lock.release();

		tp->tv_sec = ( long ) ( t_val / freq );

		ulong rest_cyc = t_val % freq;
		double rest = ( double ) rest_cyc * ( double ) _1G / ( double ) freq;

		tp->tv_nsec = ( long ) rest;

		return 0;
	}

} // namespace tm

