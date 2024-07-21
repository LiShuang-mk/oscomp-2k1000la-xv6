//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "hsai_defs.h"

namespace hsai
{
	class VirtualDevice;
	class BlockDevice;
	class CharDevice;

	struct DeviceTableEntry
	{
		// enum DeviceTableEntryType
		// {
		// 	unknown_dev,
		// 	block_dev,
		// 	char_dev,
		// } type;
		VirtualDevice * device_ptr;
		const char * device_name;
	};

	class DeviceManager
	{
	private:
		DeviceTableEntry _device_table[ DEV_TBL_LEN ] = {};

		static const char * _device_default_name;

	public:
		DeviceManager();
		int register_device( VirtualDevice * dev, const char * name );
		int register_block_device( BlockDevice * bd, const char * name );
		int register_char_device( CharDevice * cd, const char * name );
		VirtualDevice * get_device( const char * dev_name );
		BlockDevice * get_block_device( const char * dev_name );
		CharDevice * get_char_device( const char * dev_name );
		int search_device( const char * name );
		int search_block_device( const char * name );
		int search_char_device( const char * name );
		int remove_block_device( const char * name );
		int remove_char_device( const char * name );

	public:

		VirtualDevice * get_device( uint dev_num )
		{
			if ( dev_num >= DEV_TBL_LEN ) return nullptr;
			return _device_table[ dev_num ].device_ptr;
		}

		int remove_device( VirtualDevice * dev )
		{
			for ( int i = 0; i < DEV_TBL_LEN; ++i )
			{
				DeviceTableEntry &te = _device_table[ i ];
				if ( te.device_ptr ==  dev )
				{
					_reset_device_table_entry( &te );
					return i;
				}
			}
			return -1;
		}

	private:

		int _search_null_device()
		{
			for ( int i = 0; auto & te : _device_table )
			{
				if ( te.device_ptr == nullptr ) return i;
				++i;
			}
			return -1;
		}

		void _reset_device_table_entry( DeviceTableEntry * dte )
		{
			dte->device_ptr = nullptr;
			dte->device_name = _device_default_name;
		}

	};
} // namespace hsai
