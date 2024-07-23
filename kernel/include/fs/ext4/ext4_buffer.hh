//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

/******************************************
 * name : Ext4Buffer
 * desc : 这是Ext4驱动内部的Buffer层
 *        用来解决与OS的Buffer块大小
 *        不一致的问题
 ******************************************/

#include <smp/spin_lock.hh>

namespace fs
{
	namespace ext4
	{
		class Ext4BufferPool;
		class Ext4FS;

		struct Ext4BufferFlag
		{
			u8 pinned : 1;
			u8 valid : 1;
			u8 _rsv : 6;
		}__attribute__( ( __packed__ ) );
		static_assert( sizeof( Ext4BufferFlag ) == 1 );

		class Ext4Buffer
		{
			friend Ext4BufferPool;
		private:
			void * _data = nullptr;				// 动态分配空间，其大小与ext4初始化读出的块大小一致（通常为4KiB）
			Ext4BufferFlag _flag;
			long _block_no = -1;				// 在ext4中的块号
			Ext4Buffer * _next = nullptr;
			Ext4Buffer * _prev = nullptr;

		public:
			Ext4Buffer() = default;
			Ext4Buffer( long buf_size );
			~Ext4Buffer() { _delete_data(); }

			void * get_data_ptr() const { return _data; }
			bool is_pinned() const { return _flag.pinned; }
			void pin() { _flag.pinned = 1; }
			void unpin() { _flag.pinned = 0; }

		private:
			void _new_data( long size ) { _data = ( void * ) new char[ size ]; }
			void _delete_data() { delete[]( char* )_data; }
		};

		constexpr long ext4_buffer_pool_default_count = 32;

		class Ext4BufferPool
		{
		private:
			hsai::SpinLock _lock;
			
			long _block_size = 0;
			Ext4Buffer _list_head;
			long _list_len = 0;
			Ext4FS * _belong_fs = nullptr;

		public:
			Ext4BufferPool() = default;
			Ext4BufferPool( long block_size, Ext4FS * fs );

			Ext4Buffer * request_block( long block_no );

		private:
			void _insert_front( Ext4Buffer * buf )
			{
				buf->_next = _list_head._next;
				buf->_prev = &_list_head;
				_list_head._next->_prev = buf;
				_list_head._next = buf;

				_list_len++;
			}
			void _insert_back( Ext4Buffer * buf )
			{
				buf->_prev = _list_head._prev;
				buf->_next = &_list_head;
				_list_head._prev->_next = buf;
				_list_head._prev = buf;

				_list_len++;
			}
			void _remove( Ext4Buffer * buf )
			{
				buf->_prev->_next = buf->_next;
				buf->_next->_prev = buf->_prev;

				_list_len--;
			}

		private:
			Ext4Buffer * _search_buffer( long block_no );
			Ext4Buffer * _alloc_buffer();
		};
	} // namespace ext4

} // namespace fs
