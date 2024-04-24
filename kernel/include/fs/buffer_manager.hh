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
	constexpr uint block_per_pool = ( uint64 ) CommonSize::_1M * 4 / ( uint64 ) mm::pg_size;

	class BufferManager
	{
	private:
		smp::Lock _lock;
		BufferBlock _buffer_pool[ block_per_pool ];

	public:
		BufferManager() {};
		void init( const char *lock_name );

		Buffer get_buffer( uint dev, uint64 lba );
	};

	extern BufferManager k_bufm;
} // namespace fs
