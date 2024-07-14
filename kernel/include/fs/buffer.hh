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
#include "klib/common.hh"

#include <mem/page.hh>

namespace fs
{
	constexpr uint default_buffer_size = hsai::page_size/*bytes*/;
	constexpr uint max_buffer_per_block = 64;

	// it is simplified here, which should be read from SATA device.
	constexpr uint default_sector_size = 512;
	constexpr uint sector_per_buffer = default_buffer_size / default_sector_size;
	constexpr uint sector_per_buffer_shift = [] () -> uint { uint _k = 0; for ( ; 1U << _k != sector_per_buffer; ++_k ); return _k; }( );
	constexpr uint sector_per_buffer_mask = sector_per_buffer - 1;
	static_assert( ( sector_per_buffer & sector_per_buffer_mask ) == 0, "sector-per-buffer should be a power of 2." );

	class BufferBlock;
	class BufferManager;
	class BufferNode;

	class Buffer
	{
		friend BufferManager;
		friend BufferBlock;
		friend BufferNode;
	private:
		void *_buffer_base;
		uint _block_number;
		uint _buffer_index;
	public:
		Buffer();
		Buffer( uint64 buffer_base, uint number, uint index );
		Buffer( void *buffer_base, uint number, uint index );

		const void * get_data_ptr()
		{
			return ( const void * ) _buffer_base;
		}

		const void * get_end_ptr()
		{
			return ( const void * ) ( ( uint64 ) _buffer_base + default_buffer_size );
		}

	// for debugging
	public:
		uint64 debug_get_buffer_base() { return ( uint64 ) _buffer_base; }
	};

	class BufferNode
	{
		friend BufferBlock;
		friend BufferManager;
	private:
		int _buf_index;
		BufferNode *_next;
		BufferNode *_prev;
	public:
		BufferNode();
	};

	/// @brief 每个block拥有一个页面的空间
	/// @details 每个buffer都是512byte，4K页面可以有8个buffer
	class BufferBlock
	{
		friend BufferManager;
	private:
		uint64 _valid_map;										// 对应位为1表示该buffer是最新的数据，可直接读写
		uint64 _dirty_map;										// 对应位为1表示该buffer的数据是脏数据，应当写回硬盘
		uint64 _disk_own_map;									// 对应位为1表示硬盘DMA正在使用这个buffer，内核不应该在此时修改对应的buffer
		int _device[ max_buffer_per_block ];					// buffer对应的虚拟设备号
		int _ref_cnt[ max_buffer_per_block ];					// buffer引用计数
		uint64 _tag_number[ max_buffer_per_block ];				// 块号归属当前块的唯一标识，实际上是LBA的高位
		pm::SleepLock _sleep_lock[ max_buffer_per_block ];		// 每个buffer均有一个睡眠锁，实际上是Manager使用和管理的

		BufferNode _nodes[ max_buffer_per_block ];				// buffer 链表节点
		BufferNode _node_head;									// buffer 链表头

		hsai::SpinLock _lock;
		uint _block_number;										// 当前block的块号
		uint _current_buffer_counts;							// 当前block拥有的buffer数量，是实际上的该块拥有的buffer数，可以动态更新
		void *_buffer_base[ max_buffer_per_block ];				// buffer的基地址
	public:
		BufferBlock() {};
		BufferBlock( const BufferBlock &b ) = delete;
		void init( uint block_number );

		BufferNode* search_buffer( int dev, uint block_num, uint64 tag_num );

		Buffer get_buffer( BufferNode* buf_node );

		BufferNode* alloc_buffer( int dev, uint block_num, uint64 tag_num );
	};
} // namespace fs
