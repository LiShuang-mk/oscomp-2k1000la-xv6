//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "trap_wrapper.hh"
#include "exception_manager.hh"


extern "C" {
	void _wrp_kernel_trap( void )
	{
		loongarch::k_em.kernel_trap();
	}

	void _wrp_machine_trap( void )
	{
		loongarch::k_em.machine_trap();
	}

	void _wrp_user_trap()
	{
		// [[maybe_unused]] uint64 test_estat;
		// asm volatile( "csrrd %0, 0x5" : "=r" ( test_estat ) : );
		loongarch::k_em.user_trap();
	}

	void _wrp_user_trap_ret( void )
	{
		loongarch::k_em.user_trap_ret();
	}
}