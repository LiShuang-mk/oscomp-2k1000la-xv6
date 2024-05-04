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
#include "klib/common.hh"

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

		Buffer read( int dev, uint lba );

		Buffer read_sync( int dev, uint lba );

		/// @brief 同步方法，比较费时，应当在调度开始前供内核使用
		void _release_buffer_sync( Buffer &buf )
		{
			_release_buffer( buf, false );
		}
		/// @brief 异步方法，允许进程间调度
		void _release_buffer( Buffer &buf )
		{
			_release_buffer( buf, true );
		}

	private:

		/// @brief 将device映射为SATA端口号
		/// @details todo: 
		///          此处的函数只是简单的恒同映射
		///          在后续完善设备管理后可能发生变化
		/// @param dev 虚拟设备号
		/// @return SATA端口号，超过32是不合法的
		int _dev_to_sata_port( int dev );

		/// @brief 尝试访问buffer，buffer不存在会分配一个可用的buffer。这个方法通常不应该直接调用，应当使用async/sync后缀的方法。
		/// @param need_sleep_lock 是否需要 sleep-lock，通常而言，这个用于异步场景
		/// @return 
		Buffer _get_buffer( int dev, uint64 lba, bool need_sleep_lock );

		/// @brief 每次 get buffer 并使用完后，应当将其释放
		/// @param used_sleep_lock 是否使用了 sleep-lock，这与 _get_buffer() 方法是一致的
		void _release_buffer( Buffer &buf, bool used_sleep_lock );


		/// @brief 同步方法，比较费时，应当在调度开始前供内核使用
		Buffer _get_buffer_sync( int dev, uint64 lba )
		{
			return _get_buffer( dev, lba, false );
		}
		/// @brief 异步方法，允许进程间调度
		Buffer _get_buffer_async( int dev, uint64 lba )
		{
			return _get_buffer( dev, lba, true );
		}

		// 对buffer块号和索引号进行断言
		void _assert_blk_or_idx( uint blk, uint idx )
		{
			assert( blk < block_per_pool,
				"块号不合法: %d ( 0 ~ %d )",
				blk, block_per_pool - 1
			);
			assert( idx < _buffer_pool[ blk ]._current_buffer_counts,
				"buffer索引不合法: %d ( 0 ~ %d )",
				idx, _buffer_pool[ blk ]._current_buffer_counts - 1
			);
		}

		// 测试对应buffer是否有效
		bool _buf_is_valid( uint blk, uint idx );
		// 测试对应buffer是否有脏数据
		bool _buf_is_dirty( uint blk, uint idx );
		// 测试对应buffer是否被硬盘持有
		bool _buf_is_disk_own( uint blk, uint idx );
		// 对应buffer设置为有效
		void _buf_set_valid( uint blk, uint idx );
		// 对应buffer设置为无效
		void _buf_reset_valid( uint blk, uint idx );
		// 对应buffer设置脏位
		void _buf_set_dirty( uint blk, uint idx );
		// 对应buffer复位脏位
		void _buf_reset_dirty( uint blk, uint idx );
		// 对应buffer设置硬盘持有位
		void _buf_set_disk_own( uint blk, uint idx );
		// 对应buffer复位硬盘持有位
		void _buf_reset_disk_own( uint blk, uint idx );

	};

	extern BufferManager k_bufm;
} // namespace fs
