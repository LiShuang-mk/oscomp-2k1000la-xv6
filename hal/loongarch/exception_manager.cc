//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "loongarch.hh"
#include "la_cpu.hh"

#include "exception_manager.hh"
#include "trap_wrapper.hh"

// #include "kernel/tm/timer_manager.hh"

// #include "kernel/pm/process.hh"
#include "trap_frame.hh"
// #include "kernel/pm/process_manager.hh"

// #include "kernel/mm/memlayout.hh"
// #include "kernel/mm/page_table.hh"

// #include "kernel/fs/dev/ahci_controller.hh"

// #include "syscall/syscall_handler.hh"

// #include <kernel/klib/common.hh>

#include <hsai_global.hh>
#include <hsai_log.hh>
#include <virtual_cpu.hh>
#include <syscall_interface.hh>
#include <process_interface.hh>
#include <timer_interface.hh>
#include <mem/page.hh>
#include <mem/virtual_memory.hh>
#include <mem/virtual_page_table.hh>
#include <intr/virtual_interrupt_manager.hh>

extern "C" {
#include "la_estat.h"

	extern void kernelvec();
	extern void handle_tlbr();
	extern void handle_merr();
	extern void uservec();
	extern void userret( uint64, uint64 );
}

namespace loongarch
{
	ExceptionManager k_em;
	char _user_or_kernel;

	void ExceptionManager::init( const char *lock_name )
	{
		_lock.init( lock_name );

		for ( auto &f : _exception_handlers )
		{
			f = [] ( uint32 ) -> void
			{
				hsai_panic( "not implement" );
			};
		}

		uint32 ecfg_data =
			( 0x0U << csr::ecfg_vs_s ) |
			( csr::itr_hwi_m << csr::ecfg_lie_s ) |
			( csr::itr_ti_m << csr::ecfg_lie_s );
		// uint32 ecfg_data =
		// 	( 0x0U << csr::ecfg_vs_s ) |
		// 	( csr::itr_hwi_m << csr::ecfg_lie_s );

		Cpu * cpu = Cpu::get_la_cpu();

		cpu->write_csr( csr::ecfg, ecfg_data );
		cpu->write_csr( csr::eentry, ( uint64 ) kernelvec );
		cpu->write_csr( csr::tlbrentry, ( uint64 ) handle_tlbr );
		cpu->write_csr( csr::merrentry, ( uint64 ) handle_merr );

		_init_exception_handler();

		// syscall::k_syscall_handler.init();

		// cpu->intr_on();
	}

	static int kernel_trap_cnt = 0;

	void ExceptionManager::kernel_trap()
	{
		// tmm::k_tm.close_ti_intr();

		// hsai_info( "enter kernel trap" );
		// printf( "\033[33m k trap \033[0m" );
		Cpu * cpu = Cpu::get_la_cpu();
		// cpu->intr_off();

		[[maybe_unused]] u32 crmd = ( u32 ) cpu->read_csr( csr::crmd );
		if ( crmd & ( 1 << 2 ) )
		{
			hsai_panic( "kernel trap : intr on!" );
		}

		kernel_trap_cnt++;

		if ( kernel_trap_cnt > 4 )
			hsai_panic( "kernel trap" );

		ulong era = cpu->read_csr( csr::era );
		ulong prmd = cpu->read_csr( csr::prmd );

		uint32 estat = ( uint32 ) cpu->read_csr( csr::estat );
		[[maybe_unused]] uint32 ecfg = ( uint32 ) cpu->read_csr( csr::ecfg );

		if ( ( ( prmd & csr::prmd_pplv_m ) >> csr::prmd_pplv_s ) != 0 )
			hsai_panic( "kerneltrap: not from privilege0" );
		if ( cpu->is_interruptible() )
			hsai_panic( "kerneltrap: interrupts enabled" );

		void * proc = nullptr;
		proc = hsai::get_cur_proc();

		if ( estat & ecfg & ( csr::itr_hwi_m | csr::itr_ti_m ) )
		{
			int which_dev = dev_intr();
			if ( ( ( which_dev & ( 1 << 1 ) ) != 0 ) && proc && hsai::proc_is_running( proc ) )
			{
				hsai::sched_proc( proc );
			}
		}

		if ( estat & csr::Estat::estat_ecode_m )
		{
			uint ecode = ( estat & csr::Estat::estat_ecode_m ) >> csr::Estat::estat_ecode_s;
			hsai_assert( ecode < _LA_ECODE_MAX_NUM_, "unknown ecode : %d", ecode );
			hsai_info( _la_ecode_spec_[ ecode ] );
			_user_or_kernel = 'k';
			// printf( "%d", ecode );
			_exception_handlers[ ecode ]( estat );
			// hsai_panic( "not implement" );}
		}

		// tmm::k_tm.open_ti_intr();

		cpu->write_csr( csr::era, era );
		cpu->write_csr( csr::prmd, prmd );


		kernel_trap_cnt--;

		// cpu->intr_on();
	}

	void ExceptionManager::user_trap()
	{
		// printf( "\033[32m u trap \033[0m" );
		// tmm::k_tm.close_ti_intr();
		Cpu *cpu = Cpu::get_la_cpu();
		u64 estat = cpu->read_csr( csr::estat );

		int which_dev = 0;
		uint64 dbg_prmd = cpu->read_csr( csr::CsrAddr::prmd );
		if ( ( dbg_prmd & 0x03 ) == 0 )
			hsai_panic( "Usertrap: not from user mode" );

		// 
		cpu->write_csr( csr::CsrAddr::eentry, ( uint64 ) kernelvec );

		// proc->set_user_ticks(
		// 	proc->get_user_ticks() +
		// 	( hsai::get_ticks() - proc->get_last_user_tick() )
		// );

		void * proc = hsai::get_cur_proc();
		TrapFrame* trapframe = ( TrapFrame* ) hsai::get_trap_frame_from_proc( proc );
		trapframe->era = cpu->read_csr( csr::CsrAddr::era );

		// printf( "\nestat=0x%x a7=%d era=0x%x\n", estat, trapframe->a7, cpu->read_csr( csr::era ) );

		if ( ( estat & csr::Estat::estat_is_m ) )
		{
// device trap
			which_dev = dev_intr();
			// ok
		}

		if ( estat & csr::Estat::estat_ecode_m )
		{

			uint ecode = ( estat & csr::Estat::estat_ecode_m ) >> csr::Estat::estat_ecode_s;
			if ( ecode != 0xb )
			{
				hsai_error( "unexcepted usertrapcause estat=%x pid=%d\n, era=%p",
					cpu->read_csr( csr::CsrAddr::estat ),
					hsai::get_pid( proc ),
					cpu->read_csr( csr::CsrAddr::era ) );
				hsai_assert( ecode < _LA_ECODE_MAX_NUM_, "" );
				hsai_info( _la_ecode_spec_[ ecode ] );
			}

			_user_or_kernel = 'u';

			_exception_handlers[ ecode ]( estat );
			// pm::k_pm.kill_proc( proc );
		}

		if ( hsai::proc_is_killed( proc ) )
			hsai::exit_proc( proc, -1 );

		if ( ( ( which_dev & ( 1 << 1 ) ) != 0 ) && proc && hsai::proc_is_running( proc ) )
		{
			hsai::sched_proc( proc );
		}

		// tmm::k_tm.open_ti_intr();

		user_trap_ret();
	}

	void ExceptionManager::user_trap_ret()
	{
		Cpu *cur_cpu = Cpu::get_la_cpu();

		//turn off interrupts until back to user space
		cur_cpu->intr_off();

		cur_cpu->write_csr( csr::CsrAddr::eentry, ( uint64 ) uservec );

		void * cur_proc = hsai::get_cur_proc();
		TrapFrame* trapframe = ( TrapFrame * ) hsai::get_trap_frame_from_proc( cur_proc );
		trapframe->kernel_pgdl = cur_cpu->read_csr( csr::CsrAddr::pgdl );
		trapframe->kernel_sp = hsai::get_kstack_from_proc( cur_proc ) + hsai::get_kstack_size( cur_proc );
		trapframe->kernel_trap = ( uint64 ) &_wrp_user_trap;
		trapframe->kernel_hartid = cur_cpu->get_cpu_id();

		uint32 x = ( uint32 ) cur_cpu->read_csr( csr::CsrAddr::prmd );
		x |= ( 0x3U << csr::Prmd::prmd_pplv_s );  // set priv to 3, user mode
		x |= csr::Prmd::prmd_pie_m;					// enable interrupts in user mode
		cur_cpu->write_csr( csr::CsrAddr::prmd, x );

		cur_cpu->write_csr( csr::era, trapframe->era );

		// cur_proc->set_last_user_tick( hsai::get_ticks() );

		volatile uint64 pgdl = hsai::get_pgd_addr( cur_proc );

		userret( hsai::get_trap_frame_vir_addr(), pgdl );
	}



	int ExceptionManager::dev_intr()
	{
		Cpu * cpu = Cpu::get_la_cpu();
		uint64 estat = cpu->read_csr( csr::estat );
		uint64 ecfg = cpu->read_csr( csr::ecfg );

		int rc;
		int dev = 0;

		if ( estat & ecfg & csr::itr_hwi_m )
		{
			rc = hsai::k_im->handle_dev_intr();
			if ( rc < 0 )
			{
				hsai_error( "im handle dev intr fail" );
				return rc;
			}
			dev |= 1 << 0;
		}

		if ( estat & ecfg & csr::itr_ti_m )
		{
			rc = 0;
			if ( cpu->get_cpu_id() == 0 )
			{
				rc = hsai::handle_tick_intr();
				if ( rc < 0 )
				{
					hsai_error( "tm handle dev intr fail" );
					return rc;
				}
			}

			cpu->write_csr(
				csr::ticlr,
				cpu->read_csr( csr::ticlr ) | 1
			);
			dev |= 1 << 1;
		}

		if ( estat & ecfg & csr::itr_ipi_m )
		{
			return -101;
		}

		if ( estat & ecfg & csr::itr_pmi_m )
		{
			return -201;
		}

		// else
		// {
		// 	// hsai_error(
		// 	// 	"unkown exception.\n"
		// 	// 	"estat: %x\n"
		// 	// 	"badv: %x\n"
		// 	// 	"badi: %x\n",
		// 	// 	estat,
		// 	// 	cpu->read_csr( csr::badv ),
		// 	// 	cpu->read_csr( csr::badi )
		// 	// );
		// 	return -101;
		// }

		return dev;
	}


	void ExceptionManager::machine_trap()
	{
		hsai_panic( "not implement" );
	}

	void ExceptionManager::ahci_read_handle()
	{
		// todo:
		// 正式的中断处理函数完成后应当删除simple intr handle
		// dev::ahci::k_ahci_ctl.simple_intr_handle();

		// dev::ahci::k_ahci_ctl.intr_handle();
		hsai_panic( "ahci intr" );
	}

// ---------------- private helper functions ----------------

	// void ExceptionManager::handle_pif()
	// {
	// 	hsai_panic(
	// 		"handle PIF :\n"
	// 		"    badv : %x"
	// 		"    badi : %x",
	// 		cpu->read_csr( csr::badv ),
	// 		cpu->read_csr( csr::badi )
	// 	);
	// }

	void ExceptionManager::_init_exception_handler()
	{
		_exception_handlers[ csr::ecode_int ] = std::bind( &ExceptionManager::dev_intr, this );

		_exception_handlers[ csr::ecode_brk ] = [] ( uint32 ) ->void
		{
			Cpu *cpu = Cpu::get_la_cpu();
			//uint64 estat = cpu->read_csr( csr::estat );
			//uint64 ecfg = cpu->read_csr( csr::ecfg );

			hsai_panic( "exception BRK:\n"
				"era: %p\n"
				"badv: %p\n"
				"badi: %p\n",
				"crmd: %p\n",
				cpu->read_csr( csr::era ),
				cpu->read_csr( csr::badv ),
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd )
			);
			return;
		};

		_exception_handlers[ csr::ecode_pil ] = [ this ] ( uint32 estat ) ->void
		{
			// printf( ( _user_or_kernel == 'u' ) ? "u" : "k" );
			// printf( "PIL" );
			// printf( "0x%x", cpu->read_csr( csr::era ) );
			Cpu * cpu = Cpu::get_la_cpu();
			[[maybe_unused]] uint64 badv = cpu->read_csr( csr::badv );
			[[maybe_unused]] uint64 era = cpu->read_csr( csr::era );

			void * proc = hsai::get_cur_proc();
			// [[maybe_unused]] hsai::Pte pte = hsai::get_pt_from_proc( proc )->walk( badv, false );
			TrapFrame *tf = ( TrapFrame* ) hsai::get_trap_frame_from_proc( proc );
			[[maybe_unused]] uint64 usp = tf->sp;

			hsai_printf( YELLOW_COLOR_PRINT "load bad virtual address : %p\n" CLEAR_COLOR_PRINT, badv );

			// hsai_printf( YELLOW_COLOR_PRINT "print the page at address a0(%p)\n" CLEAR_COLOR_PRINT, tf->a0 );
			// this->_print_va_page( proc, tf->a0 );

			if ( ( era >> 60 ) == 0 )
				this->_print_va_page( proc, era );
			else
				this->_print_pa_page( era );
			this->_print_va_page( proc, usp );

			this->_print_trap_frame( proc );

			// ulong iofbadv = this->_get_user_data( proc, badv );
			// hsai_printf( BLUE_COLOR_PRINT "read data(u64) from badv %p = %#lx\n" CLEAR_COLOR_PRINT,
			// 	badv, iofbadv );

			hsai_panic(
				"handle exception PIL :\n"
				"    badv : %p\n"
				"    badi : %#010x\n"
				"    crmd : %#010x\n"
				"    era  : %p\n"
				"    tick : %d\n"
				"    sp   : %p\n",
				badv,
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd ),
				cpu->read_csr( csr::era ),
				hsai::get_ticks(),
				usp
				// pte.get_data()
			);
		};

		_exception_handlers[ csr::ecode_pis ] = [ this ] ( uint32 estat ) ->void
		{
			// [[maybe_unused]] mm::Pte pte = Cpu::get_cpu()->get_cur_proc()->get_pagetable().walk( badv, 0 );
			hsai_trace( "trace kernel trap count = %d", kernel_trap_cnt );

			Cpu * cpu = Cpu::get_la_cpu();
			[[maybe_unused]] uint64 badv = cpu->read_csr( csr::badv );
			[[maybe_unused]] uint64 era = cpu->read_csr( csr::era );

			void * proc = hsai::get_cur_proc();
			// [[maybe_unused]] hsai::Pte pte = hsai::get_pt_from_proc( proc )->walk( badv, false );
			TrapFrame *tf = ( TrapFrame* ) hsai::get_trap_frame_from_proc( proc );
			[[maybe_unused]] uint64 usp = tf->sp;

			// hsai_printf( YELLOW_COLOR_PRINT "print the page at address a0(%p)\n" CLEAR_COLOR_PRINT, tf->a0 );
			// this->_print_va_page( proc, tf->a0 );

			if ( ( era >> 60 ) == 0 )
				this->_print_va_page( proc, era );
			else
				this->_print_pa_page( era );
			this->_print_va_page( proc, usp );

			this->_print_trap_frame( proc );

			ulong iofbadv = this->_get_user_data( proc, badv );
			hsai_printf( BLUE_COLOR_PRINT "read data(u64) from badv %p = %#lx\n" CLEAR_COLOR_PRINT,
				badv, iofbadv );

			hsai::Pte pte = hsai::get_pt_from_proc( proc )->walk( badv, false );
			hsai_printf( YELLOW_COLOR_PRINT "pte corresponding to badv is %p\n" CLEAR_COLOR_PRINT,
				pte.get_data() );

			hsai_panic(
				"handle exception PIS :\n"
				"    badv : %p\n"
				"    badi : %#010x\n"
				"    crmd : %#010x\n"
				"    era  : %p\n"
				"    tick : %d\n"
				"    sp   : 0x%x\n",
				badv,
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd ),
				cpu->read_csr( csr::era ),
				hsai::get_ticks(),
				usp
			);
		};

		_exception_handlers[ csr::ecode_pif ] = [] ( uint32 ) -> void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			hsai_panic(
				"handle exception PIF :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d",
				cpu->read_csr( csr::badv ),
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd ),
				cpu->read_csr( csr::era ),
				hsai::get_ticks()
			);
		};

		_exception_handlers[ csr::ecode_pme ] = [] ( uint32 estat ) ->void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			uint64 badv = cpu->read_csr( csr::badv );
			void * proc = hsai::get_cur_proc();
			[[maybe_unused]] hsai::Pte pte = hsai::get_pt_from_proc( proc )->walk( badv, 0 );
			hsai_panic(
				"handle exception PME :\n"
				"    badv : %x\n"
				"    badi : %x\n"
				"    pte  : %p",
				badv,
				cpu->read_csr( csr::badi ),
				pte.get_data()
			);
		};

		_exception_handlers[ csr::ecode_ppi ] = [] ( uint32 estat ) ->void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			uint64 badv = cpu->read_csr( csr::badv );
			void * proc = hsai::get_cur_proc();
			[[maybe_unused]] hsai::Pte pte = hsai::get_pt_from_proc( proc )->walk( badv, 0 );
			hsai_panic(
				"handle exception PPI :\n"
				"    badv : %x\n"
				"    badi : %x\n"
				"    pte  : %x",
				badv,
				cpu->read_csr( csr::badi ),
				pte.get_data()
			);
		};

		_exception_handlers[ csr::ecode_ade ] = [ this ] ( uint32 estat ) -> void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			[[maybe_unused]] uint e_sub_code =
				( estat & ( csr::Estat::estat_esubcode_m ) )
				>> csr::Estat::estat_esubcode_s;

			u64 era = cpu->read_csr( csr::era );
			void * proc = hsai::get_cur_proc();

			u32 bad_instr;
			if ( ( era >> 60 ) == 0 )
				bad_instr = ( u32 ) this->_get_user_data( proc, era );
			else
				bad_instr = *( u32 * ) era;
			hsai_printf( BLUE_COLOR_PRINT "出错指令: %#010x\n" CLEAR_COLOR_PRINT, bad_instr );
			hsai_panic(
				"handle exception ADE :\n"
				"    type : %s\n"
				"    badv : %p\n"
				"    badi : %#010x\n"
				"    crmd : %#010x\n"
				"    era  : %p\n"
				"    tick : %d\n",
				e_sub_code ? "取指地址错误(ADEF)" : "访存指令地址错误(ADEM)",
				cpu->read_csr( csr::badv ),
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd ),
				era,
				hsai::get_ticks()
			);
		};

		_exception_handlers[ csr::ecode_pnx ] = [] ( uint32 ) -> void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			hsai_panic(
				"handle exception PNX :\n"
				"    badv : %x\n"
				"    badi : %x",
				cpu->read_csr( csr::badv ),
				cpu->read_csr( csr::badi )
			);
		};

		_exception_handlers[ csr::ecode_sys ] = std::bind( &ExceptionManager::_syscall, this );

		_exception_handlers[ csr::ecode_ine ] = [] ( uint32 ) -> void
		{
			Cpu * cpu = Cpu::get_la_cpu();
			hsai_panic(
				"handle exception INE :\n"
				"    badv : 0x%x\n"
				"    badi : 0x%x\n"
				"    crmd : 0x%x\n"
				"    era  : 0x%x\n"
				"    tick : %d",
				cpu->read_csr( csr::badv ),
				cpu->read_csr( csr::badi ),
				cpu->read_csr( csr::crmd ),
				cpu->read_csr( csr::era ),
				hsai::get_ticks()
			);
		};
	}

	void ExceptionManager::_syscall()
	{
		void * p = hsai::get_cur_proc();
		TrapFrame * tf = ( TrapFrame * ) hsai::get_trap_frame_from_proc( p );

		if ( hsai::proc_is_killed( p ) )
			hsai::exit_proc( p, -1 );

		//update pc
		tf->era += 4;

		// tmm::k_tm.close_ti_intr();

		Cpu * cpu = Cpu::get_la_cpu();
		cpu->intr_on();

		uint64 num;

		// printf( "sys a0=0x%x a7=%d\n", p->get_trapframe()->a0, p->get_trapframe()->a7 );
		num = tf->a7;
		if ( num > 0 && num < hsai::get_syscall_max_num() )
		{
			// printf( "syscall num: %d\n", num );
			tf->a0 = hsai::kernel_syscall( num );
		}
		else
		{
			hsai_printf( "%d : unknown sys call %d\n",
				hsai::get_pid( p ), num );
			tf->a0 = -1;
		}

		// tmm::k_tm.open_ti_intr();

	}

	ulong ExceptionManager::_get_user_data( void * proc, u64 virt_addr )
	{
		hsai::VirtualPageTable * pt = hsai::get_pt_from_proc( proc );
		u64 pa = pt->walk_addr( virt_addr );
		pa = hsai::k_mem->to_vir( pa );
		return *( ( ulong * ) pa );
	}

	void ExceptionManager::_print_va_page( void * proc, u64 virt_addr )
	{
		hsai::VirtualPageTable * pt = hsai::get_pt_from_proc( proc );
		u64 pa = pt->walk_addr( virt_addr );
		pa = hsai::page_round_down( pa );
		pa = hsai::k_mem->to_vir( pa );

		u8 * p = ( u8* ) pa;
		u64 va_start = hsai::page_round_down( virt_addr );

		hsai_printf( BLUE_COLOR_PRINT "print the page data containing va(%p)\n" CLEAR_COLOR_PRINT, virt_addr );
		hsai_printf( "%p(ph)  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n", pa );
		for ( uint i = 0; i < hsai::page_size; ++i )
		{
			if ( i % 0x10 == 0 )
				hsai_printf( "%p      ", va_start + i );
			hsai_printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				hsai_printf( "\n" );
		}
	}

	void ExceptionManager::_print_pa_page( u64 phys_addr )
	{
		phys_addr = hsai::k_mem->to_vir( phys_addr );
		u64 pa = hsai::page_round_down( phys_addr );

		u8 * p = ( u8* ) pa;

		hsai_printf( BLUE_COLOR_PRINT "print the page data containing pa(%p)\n" CLEAR_COLOR_PRINT, phys_addr );
		hsai_printf( "%p(ph)  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n", pa );
		for ( uint i = 0; i < hsai::page_size; ++i )
		{
			if ( i % 0x10 == 0 )
				hsai_printf( "%p      ", pa + i );
			hsai_printf( "%B ", p[ i ] );
			if ( i % 0x10 == 0xF )
				hsai_printf( "\n" );
		}
	}

	void ExceptionManager::_print_trap_frame( void * proc )
	{
		TrapFrame *tf = ( TrapFrame * ) hsai::get_trap_frame_from_proc( proc );
		hsai_printf( BLUE_COLOR_PRINT "print trap frame:\n" );
		hsai_printf( "\tra(r1)  = %p\n", tf->ra );
		hsai_printf( "\ttp(r2)  = %p\n", tf->tp );
		hsai_printf( "\tsp(r3)  = %p\n", tf->sp );
		hsai_printf( "\ta0(r4)  = %p\n", tf->a0 );
		hsai_printf( "\ta1(r5)  = %p\n", tf->a1 );
		hsai_printf( "\ta2(r6)  = %p\n", tf->a2 );
		hsai_printf( "\ta3(r7)  = %p\n", tf->a3 );
		hsai_printf( "\ta4(r8)  = %p\n", tf->a4 );
		hsai_printf( "\ta5(r9)  = %p\n", tf->a5 );
		hsai_printf( "\ta6(r10) = %p\n", tf->a6 );
		hsai_printf( "\ta7(r11) = %p\n", tf->a7 );
		hsai_printf( "\tt0(r12) = %p\n", tf->t0 );
		hsai_printf( "\tt1(r13) = %p\n", tf->t1 );
		hsai_printf( "\tt2(r14) = %p\n", tf->t2 );
		hsai_printf( "\tt3(r15) = %p\n", tf->t3 );
		hsai_printf( "\tt4(r16) = %p\n", tf->t4 );
		hsai_printf( "\tt5(r17) = %p\n", tf->t5 );
		hsai_printf( "\tt6(r18) = %p\n", tf->t6 );
		hsai_printf( "\tt7(r19) = %p\n", tf->t7 );
		hsai_printf( "\tt8(r20) = %p\n", tf->t8 );
		hsai_printf( "\tr21     = %p\n", tf->r21 );
		hsai_printf( "\tfp(r22) = %p\n", tf->fp );
		hsai_printf( "\ts0(r23) = %p\n", tf->s0 );
		hsai_printf( "\ts1(r24) = %p\n", tf->s1 );
		hsai_printf( "\ts2(r25) = %p\n", tf->s2 );
		hsai_printf( "\ts3(r26) = %p\n", tf->s3 );
		hsai_printf( "\ts4(r27) = %p\n", tf->s4 );
		hsai_printf( "\ts5(r28) = %p\n", tf->s5 );
		hsai_printf( "\ts6(r29) = %p\n", tf->s6 );
		hsai_printf( "\ts7(r30) = %p\n", tf->s7 );
		hsai_printf( "\ts8(r31) = %p\n", tf->s8 );
		hsai_printf( CLEAR_COLOR_PRINT );
	}

}// namespace loongarch

