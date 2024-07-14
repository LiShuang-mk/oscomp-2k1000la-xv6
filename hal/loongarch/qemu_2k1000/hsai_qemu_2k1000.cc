//
// Created by Li Shuang ( pseudonym ) on 2024-06-24 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "la_cpu.hh"
#include "la_mem.hh"
#include "qemu_2k1000.hh"
#include "trap_frame.hh"
#include "exception_manager.hh"
#include "context.hh"

#include <hsai_global.hh>
#include <hsai_log.hh>
#include <timer_interface.hh>
#include <process_interface.hh>
#include <uart/uart_ns16550.hh>

namespace loongarch
{
	namespace qemu2k1000
	{
		hsai::UartNs16550 debug_uart;
	} // namespace qemu2k1000

} // namespace loongarch

using namespace loongarch;

extern "C" {
	extern uint64 _start_u_init;
	extern uint64 _end_u_init;
	extern uint64 _u_init_stks;
	extern uint64 _u_init_stke;
	extern uint64 _u_init_txts;
	extern uint64 _u_init_txte;
	extern uint64 _u_init_dats;
	extern uint64 _u_init_date;
	extern int init_main( void );
}

namespace hsai
{
	const u64 memory_start = qemu2k1000::mem_start;
	const u64 memory_size = qemu2k1000::mem_size;

	const uint context_size = sizeof( loongarch::Context );

	constexpr uint64 _1K_dec = 1000UL;
	constexpr uint64 _1M_dec = _1K_dec * _1K_dec;

	// 以下两个常量表达式应当是绑定在一起的

	// 参考测例说明的时钟频率是12.5MHz, 但是测试发现比这个值要小一些，大约是四分之一
	// constexpr uint64 qemu_fre = 12 * _1M_dec + 500 * _1K_dec;
	constexpr uint64 qemu_fre = 3 * _1M_dec + 125 * _1K_dec;
	// 由cycles计算出微秒的方法
	constexpr uint64 qemu_fre_cal_usec( uint64 cycles ) { return cycles * 8 / 25; }
	// 由微秒计算出cycles的方法
	constexpr uint64 qemu_fre_cal_cycles( uint64 usec ) { return usec * 25 / 8; }

	// 100K 分频，则时间片 100K/3.125MHz ~ 32ms 
	// constexpr uint div_fre = 0x80000000UL;
	constexpr uint div_fre = ( 200 * _1K_dec ) >> 2;				// 低两位由硬件补齐
	constexpr uint ms_per_tick = div_fre * _1K_dec / qemu_fre;

	void hardware_abstract_init( void )
	{
		// 1. 使用 UART0 作为 debug 输出
		new ( &qemu2k1000::debug_uart ) UartNs16550(
			( void* ) (
				( uint64 ) qemu2k1000::UartAddr::uart0
				| ( uint64 ) qemu2k1000::dmwin::win_1
				) );
		register_debug_uart( &qemu2k1000::debug_uart );

		// 2. 架构初始化
		loongarch_init();

		// 3. 初始化 Memory
		qemu2k1000::Memory::memory_init();

		// 4. 时钟初始化

		Cpu * lacpu = ( Cpu* ) get_cpu();

		// >> 自动循环
		ulong tcfg_data =
			( ( ( uint64 ) div_fre ) << csr::tcfg_initval_s ) |
			( csr::tcfg_en_m ) |
			( csr::tcfg_periodic_m );

		// >> 非自动循环
		// _tcfg_data =
		// 	( ( ( uint64 ) div_fre ) << loongarch::csr::Tcfg::tcfg_initval_s ) |
		// 	( loongarch::csr::Tcfg::tcfg_en_m );

		lacpu->write_csr( csr::tcfg, tcfg_data );
	}

	void user_proc_init( void * proc )
	{
		TrapFrame * tf = ( TrapFrame* ) get_trap_frame_from_proc( proc );
		tf->era = ( uint64 ) &init_main - ( uint64 ) &_start_u_init;
		hsai_info( "user init: era = %p", tf->era );
		tf->sp = ( uint64 ) &_u_init_stke - ( uint64 ) &_start_u_init;
		hsai_info( "user init: sp  = %p", tf->sp );
	}

	void set_trap_frame_return_value( void * trapframe, ulong v )
	{
		TrapFrame * tf = ( TrapFrame * ) trapframe;
		tf->a0 = v;
	}

	void set_trap_frame_user_sp( void * trapframe, ulong sp )
	{
		TrapFrame * tf = ( TrapFrame * ) trapframe;
		tf->sp = sp;
	}

	void set_trap_frame_arg( void * trapframe, uint arg_num, ulong value )
	{
		TrapFrame * tf = ( TrapFrame * ) trapframe;
		switch ( arg_num )
		{
			case 0: tf->a0 = value; break;
			case 1: tf->a1 = value; break;
			case 2: tf->a2 = value; break;
			case 3: tf->a3 = value; break;
			case 4: tf->a4 = value; break;
			case 5: tf->a5 = value; break;
			case 6: tf->a6 = value; break;
			case 7: tf->a7 = value; break;
			default: hsai_panic( "invalid argument number" ); break;
		}
	}

	void copy_trap_frame( void * from, void * to )
	{
		TrapFrame * tf_f = ( TrapFrame * ) from;
		TrapFrame * tf_t = ( TrapFrame * ) to;
		*tf_t = *tf_f;
	}

	ulong get_arg_from_trap_frame( void * trapframe, uint arg_num )
	{
		TrapFrame * tf = ( TrapFrame * ) trapframe;
		switch ( arg_num )
		{
			case 0: return tf->a0;
			case 1: return tf->a1;
			case 2: return tf->a2;
			case 3: return tf->a3;
			case 4: return tf->a4;
			case 5: return tf->a5;
			default: { hsai_panic( "invalid argument number" ); return 0; }
		}
		return -1;
	}

	void user_trap_return()
	{
		k_em.user_trap_ret();
	}

	void set_context_entry( void * cont, void * entry )
	{
		Context * ct = ( Context * ) cont;
		ct->ra = ( ulong ) entry;
	}

	void set_context_sp( void * cont, ulong sp )
	{
		Context * ct = ( Context * ) cont;
		ct->sp = sp;
	}

	void * get_context_address( uint proc_gid )
	{
		if ( proc_gid >= proc_pool_size )
			return nullptr;
		return &proc_context_pool()[ proc_gid ];
	}

	ulong get_main_frequence()
	{
		return qemu_fre;
	}

	ulong get_hw_time_stamp()
	{
		return ( ( Cpu * ) hsai::get_cpu() )->read_csr( csr::tval );
	}

	ulong time_stamp_to_usec( ulong ts )
	{
		return qemu_fre_cal_usec( ts );
	}

	ulong usec_to_time_stamp( ulong us )
	{
		return qemu_fre_cal_cycles( us );
	}

	ulong cycles_per_tick()
	{
		return div_fre << 2;
	}

} // namespace hsai
