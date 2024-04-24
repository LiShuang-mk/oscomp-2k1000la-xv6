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
	constexpr uint buffer_per_block = mm::pg_size / default_buffer_size;

	class BufferBlock;
	class BufferManager;

	class Buffer
	{
		friend BufferManager;
		friend BufferBlock;
	private:
		void *_buffer_base;
		uint _block_number;
		uint _buffer_index;
	public:
		Buffer();
		Buffer( uint64 buffer_base, uint number, uint index );
		Buffer( void *buffer_base, uint number, uint index );

	// for debugging
	public:
		uint64 debug_get_buffer_base() { return ( uint64 ) _buffer_base; }
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
		uint _ref_cnt[ buffer_per_block ];
		uint64 _tag_number[ buffer_per_block ];
		pm::SleepLock _sleep_lock[ buffer_per_block ];

		smp::Lock _lock;
		uint _block_number;
		void const *_page_base;
	public:
		BufferBlock() {};
		BufferBlock( const BufferBlock &b ) = delete;
		void init( void *page_base, uint block_number );


		int search_buffer( uint dev, uint block_num, uint64 tag_num );

		Buffer get_buffer( int buffer_index );

		int alloc_buffer( uint dev, uint block_num, uint64 tag_num );
	};
} // namespace fs
