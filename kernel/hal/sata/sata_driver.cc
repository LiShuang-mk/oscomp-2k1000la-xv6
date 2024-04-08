//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/sata_driver.hh"
#include "hal/sata/hba_mem.hh"

namespace ata
{
	namespace sata
	{
		void SataDriver::init( const char *lock_name, void *clb, void *fb )
		{
			_lock.init( lock_name );
			_clb = ( HbaCmdList* ) clb;
			_fb = ( HbaRevFis* ) fb;
		}
	} // namespace sata

} // namespace ata

