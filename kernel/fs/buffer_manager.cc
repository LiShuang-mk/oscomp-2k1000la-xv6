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
			void *page = mm::k_pmm.alloc_page();
			block->init( page, i );
		}
	}

	uint BufferManager::dev_to_sata_port( uint dev )
	{
		return dev;
	}

	Buffer BufferManager::get_buffer( uint dev, uint64 lba )
	{
		_lock.acquire();

		uint block_number = lba % block_per_pool;
		uint tag_number = lba / block_per_pool;
		int buf_index = _buffer_pool[ block_number ].search_buffer( dev, block_number, tag_number );
		if ( buf_index >= 0 )
		{
			_buffer_pool[ block_number ]._ref_cnt[ buf_index ]++;

		}
		else
		{
			buf_index = _buffer_pool[ block_number ].alloc_buffer( dev, block_number, tag_number );
			if ( buf_index < 0 )
			{
				log_panic(
					"BufferManager : try to get buffer fail\n"
					"it shall sleep to wait buffer to use\n"
					"but sleep not implement"
				);
			}
		}
		_lock.release();
		_buffer_pool[ block_number ]._sleep_lock[ buf_index ].acquire();
		return _buffer_pool[ block_number ].get_buffer( buf_index );
	}

	void BufferManager::release_buffer( Buffer buf )
	{
		uint blk = buf._block_number;
		uint idx = buf._buffer_index;
		if ( !_buffer_pool[ blk ]._sleep_lock[ idx ].is_holding() )
			log_panic( "not hold buffer sleep-lock" );
		_buffer_pool[ blk ]._sleep_lock[ idx ].release();
		_lock.acquire();
		_buffer_pool[ blk ]._ref_cnt[ idx ]--;
		_lock.release();
	}

	Buffer BufferManager::read( uint dev, uint lba )
	{
		Buffer buf = get_buffer( dev, lba );
		if ( _buffer_pool[ buf._block_number ]._valid[ buf._buffer_index ] == false )
		{
			log__warn( "sleep not implement, so read disk will utilize synchronous way." );
			bool dma_finish = false;
			dev::ahci::k_ahci_ctl.isu_cmd_read_dma( dev_to_sata_port( dev ), lba, buf._buffer_base, default_buffer_size, [ & ] () -> void
			{
				dma_finish = true;
			} );
			while ( !dma_finish );
			_buffer_pool[ buf._block_number ]._valid[ buf._buffer_index ] = true;
		}
		return buf;
	}
} // namespace fs
