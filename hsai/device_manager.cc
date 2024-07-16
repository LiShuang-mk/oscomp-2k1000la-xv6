//
// Created by Li Shuang ( pseudonym ) on 2024-07-16 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "device_manager.hh"
#include "hsai_global.hh"

#include <kernel/klib/klib.hh>

namespace hsai
{
	DeviceManager k_devm;

	const char * DeviceManager::_device_default_name = "(null device)";

	DeviceManager::DeviceManager()
	{
		for ( auto & te : _block_device_table )
			_reset_device_table_entry( &te );
		for ( auto & te : _char_device_table )
			_reset_device_table_entry( &te );
	}

	int DeviceManager::register_block_device( BlockDevice * bd, const char * name )
	{
		int dtei = _search_null_block_device();
		if ( dtei < 0 )
			return -1;
		DeviceTableEntry * dte = &_block_device_table[ dtei ];
		dte->type = DeviceTableEntry::block_dev;
		dte->device_ptr = ( void * ) bd;
		dte->device_name = name;
		return dtei;
	}

	int DeviceManager::register_char_device( CharDevice * cd, const char * name )
	{
		int dtei = _search_null_char_device();
		if ( dtei < 0 )
			return -1;
		DeviceTableEntry * dte = &_char_device_table[ dtei ];
		dte->type = DeviceTableEntry::char_dev;
		dte->device_ptr = ( void * ) cd;
		dte->device_name = name;
		return dtei;
	}

	BlockDevice * DeviceManager::get_block_device( const char * dev_name )
	{
		int dev_num = search_block_device( dev_name );
		if ( dev_num < 0 )
			return nullptr;
		return get_block_device( dev_num );
	}

	CharDevice * DeviceManager::get_char_device( const char * dev_name )
	{
		int dev_num = search_char_device( dev_name );
		if ( dev_num < 0 )
			return nullptr;
		return get_char_device( dev_num );
	}

	int DeviceManager::search_block_device( const char * name )
	{
		for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
		{
			if ( _block_device_table[ i ].type == DeviceTableEntry::unknown_dev )
				continue;
			if ( strcmp( name, _block_device_table[ i ].device_name ) == 0 )
				return i;
		}
		return -1;
	}

	int DeviceManager::search_char_device( const char * name )
	{
		for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
		{
			if ( _char_device_table[ i ].type == DeviceTableEntry::unknown_dev )
				continue;
			if ( strcmp( name, _char_device_table[ i ].device_name ) == 0 )
				return i;
		}
		return -1;
	}

	int DeviceManager::remove_block_device( const char * name )
	{
		for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
		{
			if ( _block_device_table[ i ].type == DeviceTableEntry::unknown_dev )
				continue;
			if ( strcmp( name, _block_device_table[ i ].device_name ) == 0 )
			{
				_reset_device_table_entry( &_block_device_table[ i ] );
				return i;
			}
		}
		return -1;
	}

	int DeviceManager::remove_char_device( const char * name )
	{
		for ( int i = 0; i < BLK_DEV_TBL_LEN; ++i )
		{
			if ( _char_device_table[ i ].type == DeviceTableEntry::unknown_dev )
				continue;
			if ( strcmp( name, _char_device_table[ i ].device_name ) == 0 )
			{
				_reset_device_table_entry( &_char_device_table[ i ] );
				return i;
			}
		}
		return -1;
	}

// **************** private helper fuctions ****************



} // namespace hsai
