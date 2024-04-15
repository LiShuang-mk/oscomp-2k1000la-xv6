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
	void kerneltrap( void )
	{
		im::k_em.kernel_trap();
	}

	void machine_trap( void )
	{
		im::k_em.machine_trap();
	}
}