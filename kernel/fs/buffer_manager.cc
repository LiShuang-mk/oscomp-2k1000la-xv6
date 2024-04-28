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
#include "klib/common.hh"

namespace fs
{
	BufferManager k_bufm;

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

	int BufferManager::dev_to_sata_port( int dev )
	{
		return dev;
	}

	Buffer BufferManager::get_buffer( int dev, uint64 lba )
	{
		_lock.acquire();

		uint block_number = lba % block_per_pool;
		uint tag_number = lba / block_per_pool;
		BufferNode *node = _buffer_pool[ block_number ].search_buffer( dev, block_number, tag_number );
		if ( node != nullptr )
		{
			_buffer_pool[ block_number ]._ref_cnt[ node->_buf_index ]++;
		}
		else
		{
			node = _buffer_pool[ block_number ].alloc_buffer( dev, block_number, tag_number );
			if ( node == nullptr )
			{
				log_panic(
					"BufferManager : try to get buffer fail\n"
					"it shall sleep to wait buffer to use\n"
					"but sleep not implement"
				);
			}
		}
		_lock.release();
		_buffer_pool[ block_number ]._sleep_lock[ node->_buf_index ].acquire();
		return _buffer_pool[ block_number ].get_buffer( node );
	}

	void BufferManager::release_buffer( Buffer buf )
	{
		uint blk = buf._block_number;
		uint idx = buf._buffer_index;

		if ( !_buffer_pool[ blk ]._sleep_lock[ idx ].is_holding() )
			log_panic( "not hold buffer sleep-lock" );
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

	Buffer BufferManager::read( int dev, uint lba )
	{
		Buffer buf = get_buffer( dev, lba );
		if ( bit_test( ( void* ) &_buffer_pool[ buf._block_number ]._valid_map, buf._buffer_index ) == false )
		{
			log__warn( "sleep not implement, so read disk will utilize synchronous way." );

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
				dev_to_sata_port( dev ), lba, default_buffer_size, 1,
				lam1,
				lam2
			);

			while ( !dma_finish );
			bit_set( ( void* ) &_buffer_pool[ buf._block_number ]._valid_map, buf._buffer_index );
		}
		return buf;
	}


} // namespace fs
