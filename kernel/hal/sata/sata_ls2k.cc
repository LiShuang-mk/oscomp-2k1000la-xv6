//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/sata_ls2k.hh"

namespace ata
{
	namespace sata
	{
		void SataLs2k::init( const char * lock_name, void *clb, void *fb )
		{

			SataDriver::init( lock_name, clb, fb );
		}
	} // namespace sata
	
} // namespace ata
