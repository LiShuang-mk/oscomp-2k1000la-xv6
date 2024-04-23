//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace dev
{
	namespace pci
	{
		class PciDriver
		{
		private:
			smp::Lock _lock;

		public:
			PciDriver() = default;
			void init( const char *lock_name );
			void sata_init();

		private:
			void sata_identify_intr_handler();
		};

		extern PciDriver k_pci_driver;

	} // namespace pci
	
} // namespace dev
