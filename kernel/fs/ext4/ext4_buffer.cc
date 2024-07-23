//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_buffer.hh"
#include "fs/ext4/ext4_fs.hh"
#include "fs/buffer_manager.hh"

namespace fs
{
	namespace ext4
	{
		Ext4Buffer::Ext4Buffer( long buf_size )
			: Ext4Buffer()
		{
			*( u8 * ) &_flag = 0;
			_new_data( buf_size );
		}

		Ext4BufferPool::Ext4BufferPool( long block_size, Ext4FS * fs )
			: Ext4BufferPool()
		{
			if ( block_size % 512 != 0 )
			{
				log_panic( "bad block size (%d)", block_size );
			}
			_lock.init( "ext4 buffer pool" );
			_block_size = block_size;
			_list_head._next = &_list_head;
			_list_head._prev = &_list_head;
			_belong_fs = fs;

			for ( long i = 0; i < ext4_buffer_pool_default_count; ++i )
			{
				Ext4Buffer * buf = new Ext4Buffer( block_size );
				_insert_front( buf );
			}
		}

		Ext4Buffer * Ext4BufferPool::request_block( long block_no, bool pin )
		{
			Ext4Buffer * pbuf = _search_buffer( block_no );

			_lock.acquire();
			if ( pbuf == nullptr )		// 没有缓存，需要重新分配并读取
			{
				// 分配一个新buffer

				pbuf = _alloc_buffer();
				if ( pbuf == nullptr )
				{
					log_warn( "ext4 buffer : no buffer to allocate" );
					_lock.release();
					return nullptr;
				}

				// 拷贝整个块到缓存


				long num_sector = _block_size / 512;
				long block_lba = block_no * num_sector;
				ulong buf_ptr = ( ulong ) pbuf->get_data_ptr();
				for ( long i = 0; i < num_sector; ++i, buf_ptr += 512 )
				{
					_lock.release();
					Buffer buf = k_bufm.read_sync( _belong_fs->owned_device(), _belong_fs->start_lba() + block_lba + i );
					_lock.acquire();
					buf.copy_data_to( ( void* ) buf_ptr );
					k_bufm.release_buffer_sync( buf );
				}

				pbuf->_flag.valid = 1;
				pbuf->_block_no = block_no;
			}

			// LRU 算法

			_remove( pbuf );
			_insert_front( pbuf );
			
			if ( pin )
				pbuf->pin();
			
			_lock.release();
			return pbuf;
		}

		Ext4Buffer * Ext4BufferPool::_search_buffer( long block_no )
		{
			Ext4Buffer * p = _list_head._next;
			for ( ; p != &_list_head; p = p->_next )
			{
				if ( p->_flag.valid && p->_block_no == block_no )
				{
					return p;
				}
			}
			return nullptr;
		}

		Ext4Buffer * Ext4BufferPool::_alloc_buffer()
		{
			Ext4Buffer * p = _list_head._prev;
			for ( ; p != &_list_head; p = p->_prev )
			{
				if ( p->_flag.pinned == 0 )
				{
					return p;
				}
			}
			return nullptr;
		}

	} // namespace ext4

} // namespace fs
