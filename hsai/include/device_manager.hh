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
	class BlockDevice;
	class CharDevice;

	struct DeviceTableEntry
	{
		enum DeviceTableEntryType
		{
			unknown_dev,
			block_dev,
			char_dev,
		} type;
		void * device_ptr;
		const char * device_name;
	};

	class DeviceManager
	{
	private:
		DeviceTableEntry _block_device_table[ BLK_DEV_TBL_LEN ] = { };
		DeviceTableEntry _char_device_table[ CHA_DEV_TBL_LEN ] = { };
		static const char * _device_default_name;

	public:
		DeviceManager();
		int register_block_device( BlockDevice * bd, const char * name );
		int register_char_device( CharDevice * cd, const char * name );
		BlockDevice * get_block_device( const char * dev_name );
		CharDevice * get_char_device( const char * dev_name );
		int search_block_device( const char * name );
		int search_char_device( const char * name );
		int remove_block_device( const char * name );
		int remove_char_device( const char * name );

	public:
		
		BlockDevice * get_block_device( int dev_num )
		{
			if ( dev_num < 0 || dev_num >= BLK_DEV_TBL_LEN )
				return nullptr;
			return ( BlockDevice * ) _block_device_table[ dev_num ].device_ptr;
		}

		CharDevice * get_char_device( int dev_num )
		{
			if ( dev_num < 0 || dev_num >= BLK_DEV_TBL_LEN )
				return nullptr;
			return ( CharDevice * ) _char_device_table[ dev_num ].device_ptr;
		}

		int remove_block_device( BlockDevice * bd )
		{
			for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
				if ( _block_device_table[ i ].device_ptr == ( void * ) bd )
				{
					_reset_device_table_entry( &_block_device_table[ i ] );
					return 0;
				}
			return -1;
		}

		int remove_char_device( CharDevice * cd )
		{
			for ( int i = 0; i < CHA_DEV_TBL_LEN; ++i )
				if ( _char_device_table[ i ].device_ptr == ( void * ) cd )
				{
					_reset_device_table_entry( &_char_device_table[ i ] );
					return 0;
				}
			return -1;
		}

	private:

		int _search_null_block_device()
		{
			for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
				if ( _block_device_table[ i ].type == DeviceTableEntry::unknown_dev )
					return i;
			return -1;
		}

		int _search_null_char_device()
		{
			for ( int i = 0; i < CHA_DEV_TBL_LEN; ++i )
				if ( _char_device_table[ i ].type == DeviceTableEntry::unknown_dev )
					return i;
			return -1;
		}

		void _reset_device_table_entry( DeviceTableEntry * dte )
		{
			dte->type = DeviceTableEntry::unknown_dev;
			dte->device_ptr = nullptr;
			dte->device_name = _device_default_name;
		}

	};
} // namespace hsai
