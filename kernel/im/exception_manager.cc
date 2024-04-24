//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "im/exception_manager.hh"
#include "im/interrupt_manager.hh"
#include "hal/cpu.hh"
#include "fs/dev/ahci_controller.hh"
#include "klib/common.hh"

extern "C" {
#include "hal/laestat.h"

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


		loongarch::Cpu::interrupt_on();
	}

	void ExceptionManager::kernel_trap()
	{
		log__info( "enter kernel trap" );
		uint32 estat = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::estat );
		uint32 ecfg = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::ecfg );
		if ( estat & ecfg & loongarch::csr::itr_ti_m )
		{
			// log__info( "handle time intr" );
			// log_trace( "read isr : %p", loongarch::qemuls2k::read_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_isr_l ) );
			loongarch::Cpu::write_csr( loongarch::csr::ticlr, loongarch::Cpu::read_csr( loongarch::csr::ticlr ) | 1 );
			return;
		}
		else if ( estat & ecfg & loongarch::csr::itr_hwi_m )
		{
			log_trace(
				"read isr : %p\n"
				"hardware itr: %p",
				loongarch::qemuls2k::read_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_isr_l ),
				( estat & loongarch::csr::itr_hwi_m ) >> loongarch::csr::itr_hwi_s
			);
			if ( estat & loongarch::csr::itr_hwi_m & ( ( 1 << 3 ) << loongarch::csr::itr_hwi_s ) )
			{
				log__info( "SATA 中断." );
				ahci_read_handle();
				return;
			}
			else if ( estat & loongarch::csr::itr_hwi_m & ( ( 1 << 2 ) << loongarch::csr::itr_hwi_s ) )
			{
				log__info( "键盘中断" );
				k_im.clear_uart0_intr();
				return;
			}
		}
		uint ecode = ( estat & loongarch::csr::Estat::estat_ecode_m ) >> loongarch::csr::Estat::estat_ecode_s;
		assert( ecode < 0x40 );
		log__info( _la_ecode_spec_[ ecode ] );
		log_panic( "not implement" );
	}

	void ExceptionManager::machine_trap()
	{
		log_panic( "not implement" );
	}

	void ExceptionManager::ahci_read_handle()
	{
		// todo:
		// 正式的中断处理函数完成后应当删除simple intr handle
		// dev::ahci::k_ahci_ctl.simple_intr_handle();

		dev::ahci::k_ahci_ctl.intr_handle();
	}
}// namespace im

