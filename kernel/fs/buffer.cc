//
// Created by Li Shuang ( pseudonym ) on 2024-04-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/buffer.hh"
#include "mm/physical_memory_manager.hh"
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
// >>>> class BufferNode

	BufferNode::BufferNode()
		: _buf_index( -1 )
		, _next( nullptr )
		, _prev( nullptr )
	{

	}

//_______________________________________________________________
// >>>> class BufferBlock 

	void BufferBlock::init( uint block_number )
	{
		_valid_map = 0UL;
		_disk_own_map = 0UL;
		_dirty_map = 0UL;
		_current_buffer_counts = max_buffer_per_block;
		for ( int &d : _device )
			d = 0;
		for ( int &ref : _ref_cnt )
			ref = 0;
		for ( uint64 &tag : _tag_number )
			tag = ~0;
		for ( pm::SleepLock &lock : _sleep_lock )
			lock.init( "buffer-block lock", "buffer-block sleep-lock" );

		_node_head._next = &_node_head;
		_node_head._prev = &_node_head;
		_node_head._buf_index = -1;
		for ( int i = 0; i < ( int ) _current_buffer_counts; i++ )
		{
			_nodes[ i ]._buf_index = i;
			_nodes[ i ]._next = _node_head._next;
			_nodes[ i ]._prev = &_node_head;
			_node_head._next->_prev = &_nodes[ i ];
			_node_head._next = &_nodes[ i ];
		}

		_lock.init( "buffer block" );
		_block_number = block_number;

		for ( int i = 0; i < ( int ) _current_buffer_counts; ++i )
		{
			_buffer_base[ i ] = mm::k_pmm.alloc_page();
			if ( _buffer_base[ i ] == nullptr )
				log_panic(
					"[ buffer alloc ] no memory.\n"
					"block number : %d\n"
					"buffer index : %d",
					_block_number, i
				);
		}
	}

	BufferNode* BufferBlock::search_buffer( int dev, uint block_num, uint64 tag_num )
	{
		assert( block_num == _block_number, "对 buffer block 而言非法的块号, 需求 %d, 而输入 %d", _block_number, block_num );
		_lock.acquire();
		for ( BufferNode *node = _node_head._next; node != &_node_head; node = node->_next )
		{
			if ( _device[ node->_buf_index ] == dev && _tag_number[ node->_buf_index ] == tag_num )
			{
				_lock.release();
				return node;
			}
		}
		_lock.release();
		return nullptr;
	}

	Buffer BufferBlock::get_buffer( BufferNode* buf_node )
	{
		assert( buf_node->_buf_index >= 0, "非法的buffer-node : buffer index is %d", buf_node->_buf_index );
		assert( buf_node->_buf_index < ( int ) max_buffer_per_block, "非法的buffer-node : buffer index is %d", buf_node->_buf_index );

		uint64 buf_base = ( uint64 ) _buffer_base[ buf_node->_buf_index ];

		return Buffer( buf_base, _block_number, buf_node->_buf_index );
	}

	BufferNode* BufferBlock::alloc_buffer( int dev, uint block_num, uint64 tag_num )
	{
		assert( block_num == _block_number, "对 buffer block 而言非法的块号, 需求 %d, 而输入 %d", _block_number, block_num );
		_lock.acquire();
		for ( BufferNode *node = _node_head._prev; node != &_node_head; node = node->_prev )
		{
			if ( _ref_cnt[ node->_buf_index ] == 0 )
			{
				// _device[ node->_buf_index ] = dev;
				// _tag_number[ node->_buf_index ] = tag_num;
				
				// _ref_cnt[ node->_buf_index ] = 1;

				_lock.release();
				
				return node;
			}
		}
		_lock.release();
		log_warn( "BufferBlock : no buffer to alloc" );
		return nullptr;
	}
} // namespace fs
