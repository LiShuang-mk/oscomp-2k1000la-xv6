//
// Created by Li shuang ( pseudonym ) on 2024-04-14 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace dev
{
	namespace ahci
	{
		class AhciController
		{
		private:
			smp::Lock _lock;

		public:
			AhciController() = default;
			void init( const char *lock_name );

			void isu_cmd_identify();
		};

		extern AhciController k_ahci_ctl;
	} // namespace ahci
	
} // namespace dev
