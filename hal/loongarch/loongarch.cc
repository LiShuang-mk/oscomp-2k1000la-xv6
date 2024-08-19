//
// Created by Li Shuang ( pseudonym ) on 2024-07-14
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "loongarch.hh"

#include <hsai_global.hh>
#include <hsai_log.hh>

#include "exception_manager.hh"
#include "la_cpu.hh"
#include "tlb_manager.hh"
#include "trap_frame.hh"

extern "C" {
// entry.S needs one stack per CPU.
char  entry_stack[loongarch::entry_stack_size * NUMCPU] __attribute__( ( aligned( 16 ) ) );
char *entry_stack_end = entry_stack + loongarch::entry_stack_size * NUMCPU;
}

namespace loongarch
{
	void loongarch_init()
	{
		// 1. 注册 CPU
		// Cpu::cpu_init();

		// 2. 开启浮点指令，注意此处仅有 0 号CPU 执行这段代码
		Cpu	  *lacpu = (Cpu *) hsai::get_cpu();
		uint64 tmp	 = lacpu->read_csr( csr::euen );
		lacpu->write_csr( csr::euen, tmp | csr::euen_fpe_m );
		hsai_info( "enable float instruction" );

		// 3. TLB 管理初始化
		k_tlbm.init( "TLB manager" );
		hsai_info( "tlbm init" );

		// 4. 关闭非对齐访存检查
	}


	u64 get_reg_from_trap_frame( TrapFrame *tf, int reg_num )
	{
		switch ( reg_num )
		{
			case 0 : return 0; break;
			case 1 : return tf->ra; break;
			case 2 : return tf->tp; break;
			case 3 : return tf->sp; break;
			case 4 : return tf->a0; break;
			case 5 : return tf->a1; break;
			case 6 : return tf->a2; break;
			case 7 : return tf->a3; break;
			case 8 : return tf->a4; break;
			case 9 : return tf->a5; break;
			case 10: return tf->a6; break;
			case 11: return tf->a7; break;
			case 12: return tf->t0; break;
			case 13: return tf->t1; break;
			case 14: return tf->t2; break;
			case 15: return tf->t3; break;
			case 16: return tf->t4; break;
			case 17: return tf->t5; break;
			case 18: return tf->t6; break;
			case 19: return tf->t7; break;
			case 20: return tf->t8; break;
			case 21: return tf->r21; break;
			case 22: return tf->fp; break;
			case 23: return tf->s0; break;
			case 24: return tf->s1; break;
			case 25: return tf->s2; break;
			case 26: return tf->s3; break;
			case 27: return tf->s4; break;
			case 28: return tf->s5; break;
			case 29: return tf->s6; break;
			case 30: return tf->s7; break;
			case 31: return tf->s8; break;

			default: return 0;
		}
	}

	void set_reg_from_trap_frame( TrapFrame *tf, int reg_num, u64 data )
	{
		switch ( reg_num )
		{
			case 0 : break;
			case 1 : tf->ra = data; break;
			case 2 : tf->tp = data; break;
			case 3 : tf->sp = data; break;
			case 4 : tf->a0 = data; break;
			case 5 : tf->a1 = data; break;
			case 6 : tf->a2 = data; break;
			case 7 : tf->a3 = data; break;
			case 8 : tf->a4 = data; break;
			case 9 : tf->a5 = data; break;
			case 10: tf->a6 = data; break;
			case 11: tf->a7 = data; break;
			case 12: tf->t0 = data; break;
			case 13: tf->t1 = data; break;
			case 14: tf->t2 = data; break;
			case 15: tf->t3 = data; break;
			case 16: tf->t4 = data; break;
			case 17: tf->t5 = data; break;
			case 18: tf->t6 = data; break;
			case 19: tf->t7 = data; break;
			case 20: tf->t8 = data; break;
			case 21: tf->r21 = data; break;
			case 22: tf->fp = data; break;
			case 23: tf->s0 = data; break;
			case 24: tf->s1 = data; break;
			case 25: tf->s2 = data; break;
			case 26: tf->s3 = data; break;
			case 27: tf->s4 = data; break;
			case 28: tf->s5 = data; break;
			case 29: tf->s6 = data; break;
			case 30: tf->s7 = data; break;
			case 31: tf->s8 = data; break;

			default: return;
		}
	}

} // namespace loongarch

namespace hsai
{} // namespace hsai

