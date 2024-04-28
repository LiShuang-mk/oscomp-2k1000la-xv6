//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"
#include "fs/buffer.hh"

namespace fs
{
	constexpr uint block_per_pool = 64;

	class BufferManager
	{
	private:
		smp::Lock _lock;
		BufferBlock _buffer_pool[ block_per_pool ];

	public:
		BufferManager() {};
		void init( const char *lock_name );

		/// @brief 将device映射为SATA端口号
		/// @details todo: 
		///          此处的函数只是简单的恒同映射
		///          在后续完善设备管理后可能发生变化
		/// @param dev 虚拟设备号
		/// @return SATA端口号，超过32是不合法的
		int dev_to_sata_port( int dev );

		Buffer get_buffer( int dev, uint64 lba );

		void release_buffer( Buffer buf );

		Buffer read( int dev, uint lba );

		
	};

	extern BufferManager k_bufm;
} // namespace fs
