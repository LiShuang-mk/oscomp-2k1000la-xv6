//
// Created by Li Shuang ( pseudonym ) on -- 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "intr/virtual_interrupt_manager.hh"
#include "hsai_global.hh"
#include "hsai_log.hh"

namespace hsai
{
	int VirtualInterruptManager::register_interrupt_manager( VirtualInterruptManager * im )
	{
		if ( k_im != nullptr )
		{
			hsai_panic( "k_im has been registered" );
			return -1;
		}
		k_im = im;
		return 0;
	}

} // namespace hsai
