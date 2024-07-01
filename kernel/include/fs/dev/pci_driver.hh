//
// Created by Li shuang ( pseudonym ) on 2024-04-09 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include <smp/spin_lock.hh>

namespace dev
{
	namespace pci
	{
		class PciDriver
		{
		private:
			hsai::SpinLock _lock;

		public:
			PciDriver() = default;
			void init( const char *lock_name );

		private:
			void _sata_init();
			void _acpi_init();
		};

		extern PciDriver k_pci_driver;

	} // namespace pci

} // namespace dev
