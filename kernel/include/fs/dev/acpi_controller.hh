//
// Created by Li Shuang ( pseudonym ) on 2024-05-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace dev
{
	namespace acpi
	{
		class AcpiController
		{
		private:
			smp::Lock _lock;
			void * _reg_base = nullptr;

		public:
			void init( const char * lock_name, uint64 acpi_reg_base );
			void power_off();
		};

		extern AcpiController k_acpi_controller;

	} // namespace acpi
	
} // namespace dev
