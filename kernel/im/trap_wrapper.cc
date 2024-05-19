//
// Created by Li shuang ( pseudonym ) on 2024-04-05 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "im/trap_wrapper.hh"
#include "im/exception_manager.hh"

extern "C" {
	void _wrp_kernel_trap( void )
	{
		im::k_em.kernel_trap();
	}

	void _wrp_machine_trap( void )
	{
		im::k_em.machine_trap();
	}

	void _wrp_user_trap( void )
	{
		im::k_em.user_trap();
	}

	void _wrp_user_trap_ret( void )
	{
		im::k_em.user_trap_ret();
	}
}