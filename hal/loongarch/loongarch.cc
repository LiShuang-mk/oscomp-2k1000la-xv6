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

extern "C" {
// entry.S needs one stack per CPU.
char  entry_stack[loongarch::entry_stack_size * NUMCPU] __attribute__( ( aligned( 16 ) ) );
char* entry_stack_end = entry_stack + loongarch::entry_stack_size * NUMCPU;
}

namespace loongarch
{
	void loongarch_init()
	{
		// 1. 注册 CPU
		// Cpu::cpu_init();

		// 2. 开启浮点指令，注意此处仅有 0 号CPU 执行这段代码
		Cpu*   lacpu = (Cpu*) hsai::get_cpu();
		uint64 tmp	 = lacpu->read_csr( csr::euen );
		lacpu->write_csr( csr::euen, tmp | csr::euen_fpe_m );
		hsai_info( "enable float instruction" );

		// 3. TLB 管理初始化
		k_tlbm.init( "TLB manager" );
		hsai_info( "tlbm init" );

		// 4. 关闭非对齐访存检查
	}

} // namespace loongarch

namespace hsai
{

} // namespace hsai

