//
// Created by Li shuang ( pseudonym ) on 2024-04-22
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "types.hh"
#include "pm/sleep_lock.hh"
#include "mm/page.hh"

namespace fs
{
	constexpr uint default_buffer_size = 512/*bytes*/;
	constexpr uint buffer_per_block = mm::pg_size / 512;

	class BufferBlock;
	class BufferManager;

	class Buffer
	{
		friend BufferManager;
		friend BufferBlock;
	private:
		void *_buffer_base;
		uint _buffer_size = default_buffer_size;
	public:

	};

	/// @brief 每个block拥有一个页面的空间
	/// @details 每个buffer都是512byte，4K页面可以有8个buffer
	class BufferBlock
	{
		friend BufferManager;
	private:
		bool _valid[ buffer_per_block ];
		bool _disk_own[ buffer_per_block ];
		uint _device[ buffer_per_block ];
		uint _block_number;
		pm::SleepLock _sleep_lock;
		uint _ref_cnt;
		void const *_page_base;
	public:
		BufferBlock() {};
		BufferBlock( const BufferBlock &b ) = delete;
		void init( void *page_base, uint block_number );
	};
} // namespace fs
