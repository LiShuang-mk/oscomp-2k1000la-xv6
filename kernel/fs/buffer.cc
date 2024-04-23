//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/buffer.hh"

namespace fs
{
	void BufferBlock::init( void *page_base, uint block_number )
	{
		_sleep_lock.init( "buffer-block lock", "buffer-block sleep-lock" );
		for ( bool &v : _valid )
			v = false;
		for ( bool &d : _disk_own )
			d = false;
		for ( uint &d : _device )
			d = 0;
		_block_number = block_number;
		_ref_cnt = 0;
		_page_base = page_base;
	}
} // namespace fs
