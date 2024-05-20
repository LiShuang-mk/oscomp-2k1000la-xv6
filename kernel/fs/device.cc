//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/device.hh"
#include "klib/klib.hh"

namespace dev
{
	DeviceManager k_dm;

	int DeviceManager::register_device(
		uint dnum,
		DeviceOperator dread,
		DeviceOperator dwrite )
	{
		assert( dnum < max_major_dev_num, "register device : device number too big" );
		_dev_ops[ dnum ].read = dread;
		_dev_ops[ dnum ].write = dwrite;
		return 0;
	}

} // namespace dev
