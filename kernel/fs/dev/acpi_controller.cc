//
// Created by Li Shuang ( pseudonym ) on 2024-05-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/dev/acpi_controller.hh"

namespace dev
{
	namespace acpi
	{
		void AcpiController::init( const char * lock_name )
		{
			_lock.init( lock_name );
		}
	} // namespace acpi
	
} // namespace fs
