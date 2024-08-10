//
// Created by Li Shuang ( pseudonym ) on 2024-04-23
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "fs/buffer_manager.hh"

#include <block_device.hh>
#include <device_manager.hh>
#include <hsai_global.hh>

#include "mm/physical_memory_manager.hh"

namespace fs
{
	BufferManager k_bufm;

	// ________________________________________________________________
	// >>>> class BufferManager

	// -------- public interface --------

	void BufferManager::init( const char* lock_name )
	{
		_lock.init( lock_name );
		BufferBlock* block;
		for ( uint i = 0; i < block_per_pool; i++ )
		{
			block = &_buffer_pool[i];
			block->init( i );
		}
	}

	Buffer BufferManager::read( int dev, uint64 lba )
	{
		log_panic( "buffer : asynchronous read not implement" );
		// Buffer buf = _get_buffer_async( dev, lba );
		// if ( bit_test( ( void* ) &_buffer_pool[ buf._block_number
		// ]._valid_map, buf._buffer_index ) == false )
		// {
		// 	// log_warn( "sleep not implement, so read disk will utilize
		// synchronous way." );

		// 	uint blk = buf._block_number;
		// 	uint idx = buf._buffer_index;

		// 	bool dma_finish = false;

		// 	auto lam1 = [ & ] ( uint i, uint64& prb, uint32& prs ) -> void
		// 	{
		// 		if ( i > 0 )
		// 			return;
		// 		prb = ( uint64 ) _buffer_pool[ blk ]._buffer_base[ idx ];
		// 		prs = default_buffer_size;
		// 	};

		// 	auto lam2 = [ & ] () -> void
		// 	{
		// 		log_trace( "buffer : read from disk. LBA : %x", lba );
		// 		dma_finish = true;
		// 	};

		// 	dev::ahci::k_ahci_ctl.isu_cmd_read_dma(
		// 		_dev_to_sata_port( dev ), lba, default_buffer_size, 1,
		// 		lam1,
		// 		lam2
		// 	);

		// 	while ( !dma_finish );
		// 	bit_set( ( void* ) &_buffer_pool[ buf._block_number ]._valid_map,
		// buf._buffer_index );
		// }
		// return buf;
		return Buffer();
	}

	Buffer BufferManager::read_sync( int dev, uint64 lba )
	{
		Buffer buf		 = _get_buffer_sync( dev, lba );
		uint   offset	 = _offset_from_lba( lba );
		buf._buffer_base = (void*) ( (uint64) buf._buffer_base +
									 default_sector_size * offset );
		return buf;
	}

	int BufferManager::dirt_buffer( Buffer& buf )
	{
		uint blk = buf._block_number;
		uint idx = buf._buffer_index;
		if ( blk >= block_per_pool ) return -1;
		if ( idx >= _buffer_pool[blk]._current_buffer_counts ) return -2;
		_buf_set_dirty( blk, idx );
		return 0;
	}

	int BufferManager::flush_buffer( Buffer& buf ) { return -1; }

	// -------- private helper function --------

	int BufferManager::_dev_to_sata_port( int dev ) { return dev; }

	bool BufferManager::_buf_is_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( (void*) &_buffer_pool[blk]._valid_map, idx );
	}

	bool BufferManager::_buf_is_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( (void*) &_buffer_pool[blk]._dirty_map, idx );
	}

	bool BufferManager::_buf_is_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( (void*) &_buffer_pool[blk]._disk_own_map, idx );
	}

	void BufferManager::_buf_set_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( (void*) &_buffer_pool[blk]._valid_map, idx );
	}

	void BufferManager::_buf_set_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( (void*) &_buffer_pool[blk]._dirty_map, idx );
	}

	void BufferManager::_buf_set_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( (void*) &_buffer_pool[blk]._disk_own_map, idx );
	}

	void BufferManager::_buf_reset_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( (void*) &_buffer_pool[blk]._valid_map, idx );
	}

	void BufferManager::_buf_reset_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( (void*) &_buffer_pool[blk]._dirty_map, idx );
	}

	void BufferManager::_buf_reset_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( (void*) &_buffer_pool[blk]._disk_own_map, idx );
	}

	Buffer BufferManager::_get_buffer( int dev, uint64 lba,
									   bool need_sleep_lock )
	{
		if ( _check_block_device( (uint) dev ) < 0 )
		{
			log_error( "Buffer : invalid device number" );
			return Buffer();
		}

		_lock.acquire();

		uint blk		 = _blk_num_from_lba( lba );
		uint tag		 = _tag_num_from_lba( lba );
		lba				 = _lba_blk_align( lba );
		BufferNode* node = nullptr;
		uint64		buf_base;

		node = _buffer_pool[blk].search_buffer( dev, blk, tag );
		if ( node != nullptr )
		{ // 命中 buffer
			// log_info( "BufferManager : 命中buffer ([%d][%d])", blk,
			// node->_buf_index );

			if ( _buf_is_disk_own( blk, node->_buf_index ) )
			{ // 虽然命中 buffer，但是这个 buffer
			  // 被硬盘占用，应当等待硬盘解除占用
				assert( need_sleep_lock == true,
					"synchronously get buffer but disk occupies this buffer\n"
					">> maybe it is asynchronously getting buffer here?"
				);

				log_panic(
					"BufferManager : hit buffer but buffer is occupied by "
					"disk\n" ">> it should sleep here but sleep not imlement" );
			}

			_buffer_pool[blk]._ref_cnt[node->_buf_index]++;
		}
		else // 没有命中 buffer，需要分配新的buffer
		{
			node = _buffer_pool[blk].alloc_buffer( dev, blk, tag );
			assert( node != nullptr,
				"BufferManager : try to get buffer fail\n"
				"  it shall sleep to wait buffer to use\n"
				"  but sleep not implement"
			);

			if ( _buf_is_dirty( blk, node->_buf_index ) )
			{ // 这个buffer有脏数据，需要回写

				uint dev_num =
					(uint) _buffer_pool[blk]._device[node->_buf_index];
				_check_block_device( dev_num );
				hsai::BlockDevice* bd =
					(hsai::BlockDevice*) hsai::k_devm.get_device( dev_num );

				buf_base =
					(uint64) _buffer_pool[blk]._buffer_base[node->_buf_index];
				hsai::BufferDescriptor buf_des = {
					.buf_addr = buf_base, .buf_size = default_buffer_size };
				u64 lba_to_write = _lba_from_tag_blk_off(
					_buffer_pool[blk]._tag_number[node->_buf_index], blk, 0 );

				_buf_set_disk_own( blk, node->_buf_index );
				if ( need_sleep_lock )
					log_panic( "BufferManager : sleep not implement" );
				else
				{
					_lock.release();
					bd->write_blocks_sync(
						lba_to_write,
						default_buffer_size / bd->get_block_size(), &buf_des,
						1 );
					_lock.acquire();
				}

				_buf_reset_dirty( blk, node->_buf_index );
				_buf_reset_disk_own( blk, node->_buf_index );
			} // buffer is dirty

			_buffer_pool[blk]._device[node->_buf_index]		= dev;
			_buffer_pool[blk]._tag_number[node->_buf_index] = tag;
			_buffer_pool[blk]._ref_cnt[node->_buf_index]	= 1;
			_buf_reset_valid( blk, node->_buf_index );

		} // >> end if ( node == nullptr )

		if ( !_buf_is_valid( blk, node->_buf_index ) )
		{ // 这个块没有有效的数据，需要从硬盘读入

			hsai::BlockDevice* bd =
				(hsai::BlockDevice*) hsai::k_devm.get_device( (uint) dev );

			buf_base =
				(uint64) _buffer_pool[blk]._buffer_base[node->_buf_index];
			hsai::BufferDescriptor buf_des = {
				.buf_addr = buf_base, .buf_size = default_buffer_size };

			_buf_set_disk_own( blk, node->_buf_index );
			if ( need_sleep_lock )
				log_panic( "BufferManager : sleep not implement" );
			else
			{
				_lock.release();
				bd->read_blocks_sync(
					lba, default_buffer_size / bd->get_block_size(), &buf_des,
					1 );
				_lock.acquire();
			}

			_buf_reset_disk_own( blk, node->_buf_index );
			_buf_set_valid( blk, node->_buf_index );
		}

		_lock.release();
		if ( need_sleep_lock )
			_buffer_pool[blk]._sleep_lock[node->_buf_index].acquire();
		return _buffer_pool[blk].get_buffer( node );
		// return Buffer();
	}

	void BufferManager::_release_buffer( Buffer& buf, bool used_sleep_lock )
	{
		uint blk = buf._block_number;
		uint idx = buf._buffer_index;

		if ( used_sleep_lock &&
			 !_buffer_pool[blk]._sleep_lock[idx].is_holding() )
			log_panic( "not hold buffer sleep-lock" );
		if ( used_sleep_lock ) _buffer_pool[blk]._sleep_lock[idx].release();

		_lock.acquire();
		BufferBlock& block = _buffer_pool[blk];
		block._ref_cnt[idx]--;
		if ( block._ref_cnt[idx] == 0 )
		{
			BufferNode& node			  = block._nodes[idx];
			node._next->_prev			  = node._prev;
			node._prev->_next			  = node._next;
			node._next					  = block._node_head._next;
			node._prev					  = &block._node_head;
			block._node_head._next->_prev = &node;
			block._node_head._next		  = &node;
		}
		_lock.release();
	}

	int BufferManager::_check_block_device( uint dev_num )
	{
		hsai::VirtualDevice* dev = hsai::k_devm.get_device( dev_num );
		if ( dev == nullptr )
		{
			log_warn( "No.%d device is (null)", dev_num );
			return -1;
		}
		if ( dev->type() != hsai::dev_block )
		{
			log_warn( "No.%d device is not a block device", dev_num );
			return -2;
		}
		return 0;
	}

} // namespace fs
