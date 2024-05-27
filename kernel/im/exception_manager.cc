//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/cpu.hh"
#include "hal/csr.hh"
#include "hal/loongarch.hh"

#include "im/exception_manager.hh"
#include "im/interrupt_manager.hh"
#include "im/trap_wrapper.hh"

#include "tm/timer_manager.hh"

#include "pm/process.hh"
#include "pm/trap_frame.hh"
#include "pm/process_manager.hh"

#include "mm/memlayout.hh"
#include "mm/page_table.hh"

#include "fs/dev/ahci_controller.hh"

#include "syscall/syscall_handler.hh"

#include "klib/common.hh"

extern "C" {
#include "hal/laestat.h"

	extern void kernelvec();
	extern void handle_tlbr();
	extern void handle_merr();
	extern void uservec();
	extern void userret( uint64, uint64 );
}

namespace im
{
	ExceptionManager k_em;

	void ExceptionManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		for ( auto &f : _exception_handlers )
		{
			f = [] ( uint32 ) -> void
			{
				log_panic( "not implement" );
			};
		}

		uint32 ecfg_data =
			( 0x0U << loongarch::csr::ecfg_vs_s ) |
			( loongarch::csr::itr_hwi_m << loongarch::csr::ecfg_lie_s ) |
			( loongarch::csr::itr_ti_m << loongarch::csr::ecfg_lie_s );
		loongarch::Cpu::write_csr( loongarch::csr::ecfg, ecfg_data );
		loongarch::Cpu::write_csr( loongarch::csr::eentry, ( uint64 ) kernelvec );
		loongarch::Cpu::write_csr( loongarch::csr::tlbrentry, ( uint64 ) handle_tlbr );
		loongarch::Cpu::write_csr( loongarch::csr::merrentry, ( uint64 ) handle_merr );


		_init_exception_handler();

		syscall::k_syscall_handler.init();

		loongarch::Cpu::interrupt_on();
	}

	void ExceptionManager::kernel_trap()
	{
		// log_info( "enter kernel trap" );
		// printf( "\033[33m k trap \033[0m" );
		uint32 estat = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::estat );
		uint32 ecfg = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::ecfg );
		if ( estat & ecfg & loongarch::csr::itr_ti_m )
		{
			// log_info( "handle time intr" );
			// log_trace( "read isr : %p", loongarch::qemuls2k::read_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_isr_l ) );
			tmm::k_tm.handle_clock_intr();
			loongarch::Cpu::write_csr( loongarch::csr::ticlr, loongarch::Cpu::read_csr( loongarch::csr::ticlr ) | 1 );
			return;
		}
		else if ( estat & ecfg & loongarch::csr::itr_hwi_m )
		{
			// log_trace(
			// 	"read isr : %p\n"
			// 	"hardware itr: %p",
			// 	loongarch::qemuls2k::read_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_isr_l ),
			// 	( estat & loongarch::csr::itr_hwi_m ) >> loongarch::csr::itr_hwi_s
			// );
			if ( estat & loongarch::csr::itr_hwi_m & ( ( 1 << 3 ) << loongarch::csr::itr_hwi_s ) )
			{
				log_info( "SATA 中断." );
				ahci_read_handle();
				return;
			}
			else if ( estat & loongarch::csr::itr_hwi_m & ( ( 1 << 2 ) << loongarch::csr::itr_hwi_s ) )
			{
				log_info( "键盘中断" );
				k_im.clear_uart0_intr();
				return;
			}
		}
		uint ecode = ( estat & loongarch::csr::Estat::estat_ecode_m ) >> loongarch::csr::Estat::estat_ecode_s;
		assert( ecode < _LA_ECODE_MAX_NUM_, "" );
		log_info( _la_ecode_spec_[ ecode ] );
		_exception_handlers[ ecode ]( estat );
		// log_panic( "not implement" );
	}

	void ExceptionManager::user_trap( uint64 estat )
	{
		// printf( "\033[32m u trap \033[0m" );
		loongarch::Cpu *cpu = loongarch::Cpu::get_cpu();
		[[maybe_unused]] uint64 test_estat = cpu->read_csr( loongarch::csr::CsrAddr::estat );
		// estat = [] ()->uint64
		// {
		// 	uint32 x;
		// 	asm volatile( "csrrd %0, 0x5" : "=r" ( x ) );
		// 	return x;
		// }( );
		int which_dev = 0;
		uint64 dbg_prmd = cpu->read_csr( loongarch::csr::CsrAddr::prmd );
		if ( ( dbg_prmd & 0x03 ) == 0 )
			log_panic( "Usertrap: not from user mode" );

		//
		cpu->write_csr( loongarch::csr::CsrAddr::eentry, ( uint64 ) kernelvec );

		pm::Pcb *proc = cpu->get_cur_proc();

		proc->set_user_ticks(
			proc->get_user_ticks() +
			( tmm::k_tm.get_ticks() - proc->get_last_user_tick() )
		);

		pm::TrapFrame* trapframe;
		trapframe = proc->get_trapframe();
		trapframe->era = cpu->read_csr( loongarch::csr::CsrAddr::era );

		if ( ( ( estat & loongarch::csr::Estat::estat_ecode_m ) >> loongarch::csr::Estat::estat_ecode_s )
			== 0xb )
		{
//syscall
			// log_info( "syscall: %d", proc->get_trapframe()->a7 );
			// printf( "\033[34m sys %d \033[0m", proc->get_trapframe()->a7 );	<<<<<<<<<<<<<<<< 跟踪 syscall

			if ( proc->is_killed() )
				pm::k_pm.exit( -1 );

			//update pc
			proc->get_trapframe()->era += 4;

			cpu->interrupt_on();

			tmm::k_tm.close_ti_intr();

			/// @todo syscall()
			_syscall();

			tmm::k_tm.open_ti_intr();

		}
		else if ( ( which_dev = dev_intr() ) > 0 )
		{
// device trap
			// ok
		}
		else
		{
			log_error( "unexcepted usertrapcause estat=%x pid=%d\n, era=%p",
				cpu->read_csr( loongarch::csr::CsrAddr::estat ),
				proc->get_pid(),
				cpu->read_csr( loongarch::csr::CsrAddr::era ) );
			uint ecode = ( estat & loongarch::csr::Estat::estat_ecode_m ) >> loongarch::csr::Estat::estat_ecode_s;
			assert( ecode < _LA_ECODE_MAX_NUM_, "" );
			log_info( _la_ecode_spec_[ ecode ] );
			_exception_handlers[ ecode ]( estat );
			// pm::k_pm.kill_proc( proc );
		}

		if ( proc->is_killed() )
			pm::k_pm.exit( -1 );

		if ( which_dev == 2 && proc && proc->get_state() == pm::ProcState::running )
		{
			pm::k_pm.sche_proc( proc );
		}

		user_trap_ret();
	}

	void ExceptionManager::user_trap_ret()
	{
		loongarch::Cpu *cur_cpu = loongarch::Cpu::get_cpu();

		pm::Pcb *cur_proc = cur_cpu->get_cur_proc();

		//turn off interrupts until back to user space
		cur_cpu->interrupt_off();

		cur_cpu->write_csr( loongarch::csr::CsrAddr::eentry, ( uint64 ) uservec );

		pm::TrapFrame* trapframe = cur_proc->get_trapframe();
		trapframe->kernel_pgdl = cur_cpu->read_csr( loongarch::csr::CsrAddr::pgdl );
		trapframe->kernel_sp = cur_proc->get_kstack() + mm::pg_size;
		trapframe->kernel_trap = ( uint64 ) &_wrp_user_trap;
		trapframe->kernel_hartid = cur_cpu->read_tp();

		uint32 x = ( uint32 ) cur_cpu->read_csr( loongarch::csr::CsrAddr::prmd );
		x |= ( 0x3U << loongarch::csr::Prmd::prmd_pplv_s );  // set priv to 3, user mode
		x |= loongarch::csr::Prmd::prmd_pie_m;					// enable interrupts in user mode
		cur_cpu->write_csr( loongarch::csr::CsrAddr::prmd, x );

		cur_cpu->write_csr( loongarch::csr::era, trapframe->era );

		cur_proc->set_last_user_tick( tmm::k_tm.get_ticks() );

		volatile uint64 pgdl = cur_proc->get_pagetable().get_base();

		userret( mm::vml::vm_trap_frame, pgdl );
	}

	int ExceptionManager::dev_intr()
	{
		loongarch::Cpu * cpu = loongarch::Cpu::get_cpu();
		uint64 estat = cpu->read_csr(
			loongarch::csr::estat
		);
		uint64 ecfg = cpu->read_csr(
			loongarch::csr::ecfg
		);

		int rc;
		if ( estat & ecfg & loongarch::csr::itr_hwi_m )
		{
			rc = k_im.handle_dev_intr();
			if ( rc < 0 )
				log_error( "im handle dev intr fail" );
			return rc;
		}
		else if ( estat & ecfg & loongarch::csr::itr_ti_m )
		{
			rc = 0;
			if ( loongarch::Cpu::read_tp() == 0 )
			{
				rc = tmm::k_tm.handle_clock_intr();
				if ( rc < 0 )
					log_error( "tm handle dev intr fail" );
			}

			loongarch::Cpu::get_cpu()->write_csr(
				loongarch::csr::ticlr,
				loongarch::Cpu::get_cpu()->read_csr( loongarch::csr::ticlr ) | 1
			);
			return rc;
		}
		else
		{
			// log_error(
			// 	"unkown exception.\n"
			// 	"estat: %x\n"
			// 	"badv: %x\n"
			// 	"badi: %x\n",
			// 	estat,
			// 	cpu->read_csr( loongarch::csr::badv ),
			// 	cpu->read_csr( loongarch::csr::badi )
			// );
			return -101;
		}
		return -1;
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

// ---------------- private helper functions ----------------

	// void ExceptionManager::handle_pif()
	// {
	// 	log_panic(
	// 		"handle PIF :\n"
	// 		"    badv : %x"
	// 		"    badi : %x",
	// 		loongarch::Cpu::read_csr( loongarch::csr::badv ),
	// 		loongarch::Cpu::read_csr( loongarch::csr::badi )
	// 	);
	// }

	void ExceptionManager::_init_exception_handler()
	{
		_exception_handlers[ loongarch::csr::ecode_pil ] = [] ( uint32 estat ) ->void
		{
			uint64 badv = loongarch::Cpu::read_csr( loongarch::csr::badv );
			[[maybe_unused]] mm::Pte pte = loongarch::Cpu::get_cpu()->get_cur_proc()->get_pagetable().walk( badv, 0 );
			// log_warn( "出现PIS异常很可能是一个诡异的bug, 此处继续运行" );
			log_error(
				"handle exception PIL :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d\n"
				"    pte  : %p",
				badv,
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				loongarch::Cpu::read_csr( loongarch::csr::crmd ),
				loongarch::Cpu::read_csr( loongarch::csr::era ),
				tmm::k_tm.get_ticks(),
				pte.get_data()
			);
		};

		_exception_handlers[ loongarch::csr::ecode_pis ] = [] ( uint32 estat ) ->void
		{
			[[maybe_unused]] uint64 badv = loongarch::Cpu::read_csr( loongarch::csr::badv );
			// [[maybe_unused]] mm::Pte pte = loongarch::Cpu::get_cpu()->get_cur_proc()->get_pagetable().walk( badv, 0 );
			log_warn( "出现PIS异常很可能是一个诡异的bug, 此处继续运行" );
			// log_error(
			// 	"handle exception PIS :\n"
			// 	"    badv : %x\n"
			// 	"    badi : %x\n"
			// 	"    pte  : %x",
			// 	badv,
			// 	loongarch::Cpu::read_csr( loongarch::csr::badi ),
			// 	pte.get_data()
			// );
			log_error(
				"handle exception PIS :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d",
				badv,
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				loongarch::Cpu::read_csr( loongarch::csr::crmd ),
				loongarch::Cpu::read_csr( loongarch::csr::era ),
				tmm::k_tm.get_ticks()
			);
		};

		_exception_handlers[ loongarch::csr::ecode_pif ] = [] ( uint32 ) -> void
		{
			log_panic(
				"handle exception PIF :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				loongarch::Cpu::read_csr( loongarch::csr::crmd ),
				loongarch::Cpu::read_csr( loongarch::csr::era ),
				tmm::k_tm.get_ticks()
			);
		};

		_exception_handlers[ loongarch::csr::ecode_pme ] = [] ( uint32 estat ) ->void
		{
			uint64 badv = loongarch::Cpu::read_csr( loongarch::csr::badv );
			[[maybe_unused]] mm::Pte pte = loongarch::Cpu::get_cpu()->get_cur_proc()->get_pagetable().walk( badv, 0 );
			log_panic(
				"handle exception PME :\n"
				"    badv : %x\n"
				"    badi : %x\n"
				"    pte  : %p",
				badv,
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				pte.get_data()
			);
		};

		_exception_handlers[ loongarch::csr::ecode_ppi ] = [] ( uint32 estat ) ->void
		{
			uint64 badv = loongarch::Cpu::read_csr( loongarch::csr::badv );
			[[maybe_unused]] mm::Pte pte = loongarch::Cpu::get_cpu()->get_cur_proc()->get_pagetable().walk( badv, 0 );
			log_panic(
				"handle exception PPI :\n"
				"    badv : %x\n"
				"    badi : %x\n"
				"    pte  : %x",
				badv,
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				pte.get_data()
			);
		};

		_exception_handlers[ loongarch::csr::ecode_ade ] = [] ( uint32 estat ) -> void
		{
			[[maybe_unused]] uint e_sub_code =
				( estat & ( loongarch::csr::Estat::estat_esubcode_m ) )
				>> loongarch::csr::Estat::estat_esubcode_s;
			log_panic(
				"handle exception ADE :\n"
				"    type : %s\n"
				"    badv : %x\n"
				"    badi : %x\n"
				"    era  : %x",
				e_sub_code ? "取指地址错误(ADEF)" : "访存指令地址错误(ADEM)",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				loongarch::Cpu::read_csr( loongarch::csr::era )
			);
		};

		_exception_handlers[ loongarch::csr::ecode_pnx ] = [] ( uint32 ) -> void
		{
			log_panic(
				"handle exception PNX :\n"
				"    badv : %x\n"
				"    badi : %x",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi )
			);
		};

		_exception_handlers[ loongarch::csr::ecode_ine ] = [] ( uint32 ) -> void
		{
			log_panic(
				"handle exception INE :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi ),
				loongarch::Cpu::read_csr( loongarch::csr::crmd ),
				loongarch::Cpu::read_csr( loongarch::csr::era ),
				tmm::k_tm.get_ticks()
			);
		};
	}

	void ExceptionManager::_syscall()
	{
		uint64 num;
		pm::Pcb * p = loongarch::Cpu::get_cpu()->get_cur_proc();

		num = p->get_trapframe()->a7;
		if ( num > 0 && num < syscall::max_syscall_funcs_num )
		{
			// printf( "syscall num: %d\n", num );
			p->get_trapframe()->a0 = syscall::k_syscall_handler.invoke_syscaller( num );
		}
		else
		{
			printf( "%d %s: unknown sys call %d\n",
				p->get_pid(), p->get_name(), num );
			p->get_trapframe()->a0 = -1;
		}
	}

}// namespace im

