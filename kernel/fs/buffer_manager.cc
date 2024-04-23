//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/buffer_manager.hh"
#include "mm/physical_memory_manager.hh"

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
} // namespace fs
