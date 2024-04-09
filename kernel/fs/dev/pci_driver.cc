//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/pci_driver.hh"
#include "mm/physical_memory_manager.hh"

namespace dev
{
	namespace pci
	{
		PciDriver k_pci_drver;

		void PciDriver::init( const char *lock_name )
		{
			_lock.init( lock_name );

			mm::k_pmm
		}

	} // namespace pci
	
} // namespace dev
