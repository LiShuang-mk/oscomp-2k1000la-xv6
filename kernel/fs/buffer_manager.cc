//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/buffer_manager.hh"
#include "fs/dev/ahci_controller.hh"
#include "mm/physical_memory_manager.hh"

namespace fs
{
	BufferManager k_bufm;

// ________________________________________________________________
// >>>> class BufferManager

// -------- public interface --------

	void BufferManager::init( const char *lock_name )
	{
		_lock.init( lock_name );
		BufferBlock *block;
		for ( uint i = 0; i < block_per_pool; i++ )
		{
			block = &_buffer_pool[ i ];
			block->init( i );
		}
	}

	Buffer BufferManager::read( int dev, uint lba )
	{
		log_panic( "buffer : asynchronous read not implement" );
		Buffer buf = _get_buffer_async( dev, lba );
		if ( bit_test( ( void* ) &_buffer_pool[ buf._block_number ]._valid_map, buf._buffer_index ) == false )
		{
			// log_warn( "sleep not implement, so read disk will utilize synchronous way." );

			uint blk = buf._block_number;
			uint idx = buf._buffer_index;

			bool dma_finish = false;

			auto lam1 = [ & ] ( uint i, uint64& prb, uint32& prs ) -> void
			{
				if ( i > 0 )
					return;
				prb = ( uint64 ) _buffer_pool[ blk ]._buffer_base[ idx ];
				prs = default_buffer_size;
			};

			auto lam2 = [ & ] () -> void
			{
				log_trace( "buffer : read from disk. LBA : %x", lba );
				dma_finish = true;
			};

			dev::ahci::k_ahci_ctl.isu_cmd_read_dma(
				_dev_to_sata_port( dev ), lba, default_buffer_size, 1,
				lam1,
				lam2
			);

			while ( !dma_finish );
			bit_set( ( void* ) &_buffer_pool[ buf._block_number ]._valid_map, buf._buffer_index );
		}
		return buf;
	}

	Buffer BufferManager::read_sync( int dev, uint lba )
	{
		Buffer buf = _get_buffer_sync( dev, lba );
		uint offset = _offset_from_lba( lba );
		buf._buffer_base = ( void* ) ( ( uint64 ) buf._buffer_base + default_sector_size * offset );
		return buf;
	}

// -------- private helper function --------

	int BufferManager::_dev_to_sata_port( int dev )
	{
		return dev;
	}

	bool BufferManager::_buf_is_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( ( void* ) &_buffer_pool[ blk ]._valid_map, idx );
	}

	bool BufferManager::_buf_is_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( ( void* ) &_buffer_pool[ blk ]._dirty_map, idx );
	}

	bool BufferManager::_buf_is_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_test( ( void* ) &_buffer_pool[ blk ]._disk_own_map, idx );
	}

	void BufferManager::_buf_set_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( ( void* ) &_buffer_pool[ blk ]._valid_map, idx );
	}

	void BufferManager::_buf_set_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( ( void* ) &_buffer_pool[ blk ]._dirty_map, idx );
	}

	void BufferManager::_buf_set_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_set( ( void* ) &_buffer_pool[ blk ]._disk_own_map, idx );
	}

	void BufferManager::_buf_reset_valid( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( ( void* ) &_buffer_pool[ blk ]._valid_map, idx );
	}

	void BufferManager::_buf_reset_dirty( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( ( void* ) &_buffer_pool[ blk ]._dirty_map, idx );
	}

	void BufferManager::_buf_reset_disk_own( uint blk, uint idx )
	{
		_assert_blk_or_idx( blk, idx );
		return bit_reset( ( void* ) &_buffer_pool[ blk ]._disk_own_map, idx );
	}

	Buffer BufferManager::_get_buffer( int dev, uint64 lba, bool need_sleep_lock )
	{
		_lock.acquire();

		uint blk = _blk_num_from_lba( lba );
		uint tag = _tag_num_from_lba( lba );
		lba = _lba_blk_align( lba );
		BufferNode *node = nullptr;
		uint64 buf_base;
		bool dma_finish;

		// used for AHCI to set PRD
		auto set_prd = [ & ] ( uint i, uint64& prb, uint32& prs ) -> void
		{
			if ( i > 0 )
				return;
			prb = ( uint64 ) buf_base;
			prs = default_buffer_size;
		};

		node = _buffer_pool[ blk ].search_buffer( dev, blk, tag );
		if ( node != nullptr )
		{ // 命中 buffer
			log_info( "BufferManager : 命中buffer ([%d][%d])", blk, node->_buf_index );

			if ( _buf_is_disk_own( blk, node->_buf_index ) )
			{ // 虽然命中 buffer，但是这个 buffer 被硬盘占用，应当等待硬盘解除占用
				assert( need_sleep_lock == true,
					"synchronously get buffer but disk occupies this buffer\n"
					">> maybe it is asynchronously getting buffer here?"
				);

				log_panic(
					"BufferManager : hit buffer but buffer is occupied by disk\n"
					">> it should sleep here but sleep not imlement"
				);
			}

			_buffer_pool[ blk ]._ref_cnt[ node->_buf_index ]++;
		}
		else // 没有命中 buffer，需要分配新的buffer
		{
			node = _buffer_pool[ blk ].alloc_buffer( dev, blk, tag );
			assert( node != nullptr,
				"BufferManager : try to get buffer fail\n"
				"  it shall sleep to wait buffer to use\n"
				"  but sleep not implement"
			);

			if ( _buf_is_dirty( blk, node->_buf_index ) )
			{	// 这个buffer有脏数据，需要回写
				_lock.release();

				dma_finish = false;

				auto call_back = [ & ] () -> void
				{
					log_trace( "buffer : write disk. LBA : 0x%x", lba );

					if ( need_sleep_lock )
						log_panic( "BufferManager : sleep not implement" );
					else
						dma_finish = true;
				};

				_buf_set_disk_own( blk, node->_buf_index );
				buf_base = ( uint64 ) _buffer_pool[ blk ]._buffer_base[ node->_buf_index ];
				dev::ahci::k_ahci_ctl.isu_cmd_write_dma(
					_dev_to_sata_port( dev ), lba, default_buffer_size, 1,
					set_prd,
					call_back
				);

				if ( need_sleep_lock )
					log_panic( "BufferManager : sleep not implement" );
				else
					while ( !dma_finish );

				_lock.acquire();
				_buf_reset_dirty( blk, node->_buf_index );
				_buf_reset_disk_own( blk, node->_buf_index );
			} // buffer is dirty

			_buffer_pool[ blk ]._device[ node->_buf_index ] = dev;
			_buffer_pool[ blk ]._tag_number[ node->_buf_index ] = tag;
			_buffer_pool[ blk ]._ref_cnt[ node->_buf_index ] = 1;
			_buf_reset_valid( blk, node->_buf_index );

		} // >> end if ( node == nullptr )

		if ( !_buf_is_valid( blk, node->_buf_index ) )
		{	// 这个块没有有效的数据，需要从硬盘读入
			_lock.release();

			dma_finish = false;

			auto call_back = [ & ] () -> void
			{
				log_trace( "buffer : read from disk. LBA : 0x%x", lba );

				if ( need_sleep_lock )
					log_panic( "BufferManager : sleep not implement" );
				else
					dma_finish = true;
			};

			_buf_set_disk_own( blk, node->_buf_index );
			buf_base = ( uint64 ) _buffer_pool[ blk ]._buffer_base[ node->_buf_index ];
			dev::ahci::k_ahci_ctl.isu_cmd_read_dma(
				_dev_to_sata_port( dev ), lba, default_buffer_size, 1,
				set_prd,
				call_back
			);

			if ( need_sleep_lock )
				log_panic( "BufferManager : sleep not implement" );
			else
				while ( !dma_finish );

			_lock.acquire();
			_buf_reset_disk_own( blk, node->_buf_index );
			_buf_set_valid( blk, node->_buf_index );
		}

		_lock.release();
		if ( need_sleep_lock )
			_buffer_pool[ blk ]._sleep_lock[ node->_buf_index ].acquire();
		return _buffer_pool[ blk ].get_buffer( node );
	}

	void BufferManager::_release_buffer( Buffer &buf, bool used_sleep_lock )
	{
		uint blk = buf._block_number;
		uint idx = buf._buffer_index;

		if ( used_sleep_lock && !_buffer_pool[ blk ]._sleep_lock[ idx ].is_holding() )
			log_panic( "not hold buffer sleep-lock" );
		if ( used_sleep_lock )
			_buffer_pool[ blk ]._sleep_lock[ idx ].release();

		_lock.acquire();
		BufferBlock &block = _buffer_pool[ blk ];
		block._ref_cnt[ idx ]--;
		if ( block._ref_cnt[ idx ] == 0 )
		{
			BufferNode &node = block._nodes[ idx ];
			node._next->_prev = node._prev;
			node._prev->_next = node._next;
			node._next = block._node_head._next;
			node._prev = &block._node_head;
			block._node_head._next->_prev = &node;
			block._node_head._next = &node;
		}
		_lock.release();
	}

} // namespace fs
