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

		public:
			void init( const char * lock_name );
		};

	} // namespace acpi
	
} // namespace dev
