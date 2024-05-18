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
#include "hal/csr.hh"
#include "hal/loongarch.hh"
#include "fs/dev/ahci_controller.hh"
#include "klib/common.hh"
#include "pm/process.hh"
#include "pm/trap_frame.hh"
#include "mm/memlayout.hh"
#include "im/trap_wrapper.hh"
#include "pm/process_manager.hh"

extern "C" {
#include "hal/laestat.h"

	extern void kernelvec();
	extern void handle_tlbr();
	extern void handle_merr();
	extern void uservec();
	extern void userret(uint64, uint64);
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

		loongarch::Cpu::interrupt_on();
	}

	void ExceptionManager::kernel_trap()
	{
		// log_info( "enter kernel trap" );
		uint32 estat = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::estat );
		uint32 ecfg = ( uint32 ) loongarch::Cpu::read_csr( loongarch::csr::ecfg );
		if ( estat & ecfg & loongarch::csr::itr_ti_m )
		{
			// log_info( "handle time intr" );
			// log_trace( "read isr : %p", loongarch::qemuls2k::read_itr_cfg( loongarch::qemuls2k::ItrCfg::itr_isr_l ) );
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
	
	void ExceptionManager::user_trap()
	{	
		loongarch::Cpu cpu = loongarch::k_cpus[0];
		int which_dev = 0;
		if((cpu.read_csr(loongarch::csr::CsrAddr::prmd) & 0x03 ) != 0)
			log_panic("Usertrap: not from user mode");
		
		//
		cpu.write_csr(loongarch::csr::CsrAddr::eentry, (uint64)kernelvec);
		
		pm::Pcb *proc = cpu.get_cur_proc();

		pm::TrapFrame* trapframe ;
		trapframe =  proc->getTrapframe();
		trapframe->era = cpu.read_csr(loongarch::csr::CsrAddr::era);
		proc->setTrapframe(trapframe);

		if( ((cpu.read_csr(loongarch::csr::CsrAddr::estat)) 
				& loongarch::csr::Estat::estat_ecode_m >> loongarch::csr::Estat::estat_ecode_s) == 0xb){
			//syscall

			if(proc->iskilled())
				pm::k_pm.exit(-1); 
			
			//update pc
			trapframe = proc->getTrapframe();
			trapframe->era +=4;
			proc->setTrapframe(trapframe);

			cpu.interrupt_on();

			/// @todo syscall()
			//syscall();

		}
		else if( !which_dev ) {
			// device trap
		}
		else {
			log_error("unexcepted usertrapcause %x pid=%d\n, era=%p",
						cpu.read_csr(loongarch::csr::CsrAddr::estat), 
						proc->get_pid(),
						cpu.read_csr(loongarch::csr::CsrAddr::era));
			proc->kill();

		}

		if(proc->iskilled())
			pm::k_pm.exit(-1);

		user_trap_ret();
	}

	void ExceptionManager::user_trap_ret()
	{
		loongarch::Cpu cpu = loongarch::k_cpus[0];
		
		pm::Pcb *cur_proc = cpu.get_cur_proc();

		//turn off interrupts until back to user space
		cpu.interrupt_off();

		cpu.write_csr(loongarch::csr::CsrAddr::eentry, (uint64)uservec);

		pm::TrapFrame* trapframe = cur_proc->getTrapframe();
		trapframe->kernel_pgdl = cpu.read_csr(loongarch::csr::CsrAddr::pgdl);
		trapframe->kernel_sp = cur_proc->get_kstack() + mm::pg_size;
		trapframe->kernel_trap = (uint64)(usertrap);
		trapframe->kernel_hartid = cpu.read_tp();

		uint32 x = cpu.read_csr(loongarch::csr::CsrAddr::prmd);
		x |= loongarch::csr::prmd::pplv_m;  // set priv to 3, user mode
		x |= loongarch::csr::prmd::pie_m;	// enable interrupts in user mode
		cpu.write_csr(loongarch::csr::CsrAddr::prmd, x);

		volatile uint64 pgdl = (uint64)(cur_proc->get_page().get_base());

		userret(mm::vml::vm_trap_frame - (uint64)mm::pg_size, pgdl);
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

// -------- private --------

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
		_exception_handlers[ loongarch::csr::ecode_pif ] = [] ( uint32 ) -> void
		{
			log_panic(
				"handle exception PIF :\n"
				"    badv : %x\n"
				"    badi : %x",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi )
			);
		};

		_exception_handlers[ loongarch::csr::ecode_ade ] = [] ( uint32 estat ) -> void
		{
			uint e_sub_code =
				( estat & ( loongarch::csr::Estat::estat_esubcode_m ) )
				>> loongarch::csr::Estat::estat_esubcode_s;
			log_panic(
				"handle exception ADE :\n"
				"    type : %s\n"
				"    badv : %x\n"
				"    badi : %x",
				e_sub_code ? "取指地址错误(ADEF)" : "访存指令地址错误(ADEM)",
				loongarch::Cpu::read_csr( loongarch::csr::badv ),
				loongarch::Cpu::read_csr( loongarch::csr::badi )
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
	}
}// namespace im

