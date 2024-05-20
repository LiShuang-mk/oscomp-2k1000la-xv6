//
// Created by Li Shuang ( pseudonym ) on 2024-05-20 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/device.hh"
#include "fs/dev/console.hh"
#include "klib/klib.hh"
#include "klib/function.hh"

namespace dev
{
	DeviceManager k_dm;

	void DeviceManager::init()
	{
		auto dft_read = [] ( int, uint64, uint64 ) -> int
		{
			log_info( "default device read." );
			return 0;
		};
		auto dft_write = [] ( int, uint64, uint64 ) -> int
		{
			log_info( "default device write." );
			return 0;
		};
		for ( auto &op : _dev_ops )
		{
			op.read = dft_read;
			op.write = dft_write;
		}

		register_device(
			dev_console_num,
			dft_read,
			std::bind( &Console::write, &k_console, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
		);
	}

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

	int DeviceManager::read_device( uint dev_num, int mode, uint64 addr, uint64 n )
	{
		assert( dev_num < max_major_dev_num, "read device : device number too big" );
		return _dev_ops[ dev_num ].read( mode, addr, n );
	}

	int DeviceManager::write_device( uint dev_num, int mode, uint64 addr, uint64 n )
	{
		assert( dev_num < max_major_dev_num, "write device : device number too big" );
		return _dev_ops[ dev_num ].write( mode, addr, n );
	}

} // namespace dev
