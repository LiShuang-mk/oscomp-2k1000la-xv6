//
// Created by Li Shuang ( pseudonym ) on 2024-07-15 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include "virtual_device.hh"

namespace hsai
{
	struct BufferDescriptor
	{
		u64 buf_addr;
		u32 buf_size;
	};
		class BlockDevice : public VirtualDevice
	{
	public:
		BlockDevice() = default;
		virtual DeviceType type() override { return DeviceType::dev_block; }
		virtual long get_block_size() = 0;
		virtual int read_blocks_sync( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count ) = 0;
		virtual int read_blocks( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count ) = 0;
		virtual int write_blocks_sync( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count ) = 0;
		virtual int write_blocks( long start_block, long block_count, BufferDescriptor * buf_list, int buf_count ) = 0;
		virtual int handle_intr() = 0;
	};

} // namespace hsai
