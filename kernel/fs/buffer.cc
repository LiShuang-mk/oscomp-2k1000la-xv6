//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/buffer.hh"
#include "klib/common.hh"

namespace fs
{
//_______________________________________________________________
// >>>> class Buffer 

	Buffer::Buffer()
		: Buffer( ( void * ) 0, 0, 0 )
	{

	}
	Buffer::Buffer( void *base, uint number, uint index )
		: _buffer_base( base )
		, _block_number( number )
		, _buffer_index( index )
	{
	}
	Buffer::Buffer( uint64 base, uint number, uint index )
		: Buffer( ( void * ) base, number, index )
	{
	}

//_______________________________________________________________
// >>>> class BufferBlock 

	void BufferBlock::init( void *page_base, uint block_number )
	{
		for ( pm::SleepLock &lock : _sleep_lock )
			lock.init( "buffer-block lock", "buffer-block sleep-lock" );
		for ( bool &v : _valid )
			v = false;
		for ( bool &d : _disk_own )
			d = false;
		for ( uint &d : _device )
			d = 0;
		for ( uint64 &tag : _tag_number )
			tag = ~0;
		for ( uint &ref : _ref_cnt )
			ref = 0;

		_lock.init( "buffer block" );
		_block_number = block_number;
		_page_base = page_base;
	}

	int BufferBlock::search_buffer( uint dev, uint block_num, uint64 tag_num )
	{
		assert( block_num == _block_number );
		_lock.acquire();
		for ( uint i = 0; i < buffer_per_block; i++ )
		{
			if ( _device[ i ] == dev && _tag_number[ i ] == tag_num )
			{
				_lock.release();
				return i;
			}
		}
		_lock.release();
		return -1;
	}

	Buffer BufferBlock::get_buffer( int index )
	{
		assert( index >= 0 );
		assert( index < ( int ) buffer_per_block );

		uint64 buf_base = ( uint64 ) _page_base + default_buffer_size * index;

		return Buffer( buf_base, _block_number, index );
	}

	int BufferBlock::alloc_buffer( uint dev, uint block_num, uint64 tag_num )
	{
		assert( block_num == _block_number );
		_lock.acquire();
		for ( uint i = 0; i < buffer_per_block; i++ )
		{
			if ( _ref_cnt[ i ] == 0 )
			{
				_device[ i ] = dev;
				_tag_number[ i ] = tag_num;
				_valid[ i ] = 0;
				_ref_cnt[ i ] = 1;

				_lock.release();
				// _sleep_lock[ i ].acquire();
				return i;
			}
		}
		_lock.release();
		log__warn( "BufferBlock : no buffer to alloc" );
		return -1;
	}
} // namespace fs
