//
// Created by Li Shuang ( pseudonym ) on 2024-08-13
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#pragma once

#include "block_device.hh"
#include "mbr.hh"

namespace hsai
{
	class DiskPartitionDevice : public BlockDevice
	{
	private:

		BlockDevice *	_dev	   = nullptr;
		long			_start_lba = 0;
		const char *	_part_name = nullptr;
		MbrPartTypeEnum _part_type = mbr_part_empty;

	public:

		DiskPartitionDevice() = default;
		DiskPartitionDevice( BlockDevice * dev, long start_lba, const char * part_name,
							 MbrPartTypeEnum part_type )
			: _dev( dev )
			, _start_lba( start_lba )
			, _part_name( part_name )
			, _part_type( part_type ) {};

		virtual long get_block_size() override
		{
			if ( _dev )
				return _dev->get_block_size();
			else
				return 0;
		};
		virtual int read_blocks_sync( long start_block, long block_count,
									  BufferDescriptor * buf_list, int buf_count ) override
		{
			if ( _dev )
				return _dev->read_blocks_sync( _start_lba + start_block, block_count, buf_list,
											   buf_count );
			else
				return -1;
		};
		virtual int read_blocks( long start_block, long block_count, BufferDescriptor * buf_list,
								 int buf_count ) override
		{
			if ( _dev )
				return _dev->read_blocks( _start_lba + start_block, block_count, buf_list,
										  buf_count );
			else
				return -1;
		};
		virtual int write_blocks_sync( long start_block, long block_count,
									   BufferDescriptor * buf_list, int buf_count ) override
		{
			if ( _dev )
				return _dev->write_blocks_sync( _start_lba + start_block, block_count, buf_list,
												buf_count );
			else
				return -1;
		};
		virtual int write_blocks( long start_block, long block_count, BufferDescriptor * buf_list,
								  int buf_count ) override
		{
			if ( _dev )
				return _dev->write_blocks( _start_lba + start_block, block_count, buf_list,
										   buf_count );
			else
				return -1;
		};
		virtual int handle_intr() override
		{
			if ( _dev )
				return _dev->handle_intr();
			else
				return -1;
		};
		virtual bool read_ready() override
		{
			if ( _dev )
				return _dev->read_ready();
			else
				return false;
		};
		virtual bool write_ready() override
		{
			if ( _dev )
				return _dev->write_ready();
			else
				return false;
		};
	};

} // namespace hsai
