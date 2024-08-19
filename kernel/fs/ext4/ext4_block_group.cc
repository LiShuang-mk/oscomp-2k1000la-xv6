//
// Created by Li Shuang ( pseudonym ) on 2024-07-22
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_block_group.hh"

#include "fs/ext4/ext4_fs.hh"
#include "klib/klib.hh"

namespace fs
{
	namespace ext4
	{
		Ext4BlockGroup::Ext4BlockGroup( Ext4GroupDesc *gd, Ext4FS *fs )
			: _gd( *gd ), _belong_fs( fs )
		{
			// 读取块位图

			_read_block_bitmap();

			// 读取 inode 位图

			_read_inode_bitmap();

			// 读取 inode 表地址

			_cache_inode_table_block = _gd.inode_table_lo;
			if ( _belong_fs->support64bit() )
				_cache_inode_table_block |= (long) _gd.inode_table_hi << 32;
		}

		int Ext4BlockGroup::read_inode( long index, Ext4Inode &node )
		{
			long inode_per_block = _belong_fs->rBlockSize() / _belong_fs->read_inode_size();
			long block_no		 = _cache_inode_table_block + ( index / inode_per_block );
			long block_idx		 = index % inode_per_block;

			Ext4Buffer *blk_buf = _belong_fs->read_block( block_no, true ); // pin the buffer
			if ( blk_buf == nullptr )
			{
				log_error( "ext4-blockgroup : read inode fail" );
				return -1;
			}
			Ext4InodeRecord *inode_ptr	= (Ext4InodeRecord *) blk_buf->get_data_ptr();
			inode_ptr				   += block_idx;
			node						= inode_ptr->inode_data;

			// log_trace( "read inode data: " );
			// hsai_printf(
			// 	BLUE_COLOR_PRINT
			// 	"buf\t00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E " "0F\n" CLEAR_COLOR_PRINT );
			// u8 *buf = (u8 *) &node;
			// for ( uint i = 0; i < sizeof( Ext4Inode ); ++i )
			// {
			// 	if ( i % 0x10 == 0 ) hsai_printf( "%B%B\t", i >> 8, i );
			// 	hsai_printf( "%B ", buf[i] );
			// 	if ( i % 0x10 == 0xF ) hsai_printf( "\n" );
			// }
			// printf( "\n" );

			blk_buf->unpin(); // unpin the buffer
			return 0;
		}

		void Ext4BlockGroup::_read_block_bitmap()
		{
			int bms		  = ( _belong_fs->_cache_blocks_per_group + 7 ) / 8;
			_block_bitmap = new u8[bms];

			long bbma = _gd.block_bitmap_lo; // 块位图地址
			if ( _belong_fs->support64bit() ) bbma |= (long) _gd.block_bitmap_hi << 32;

			_belong_fs->read_data( bbma, (void *) _block_bitmap, bms );
		}

		void Ext4BlockGroup::_read_inode_bitmap()
		{
			int ims		  = ( _belong_fs->_cache_inodes_per_group + 7 ) / 8;
			_inode_bitmap = new u8[ims];

			long ibma = _gd.inode_bitmap_lo; // inode位图地址
			if ( _belong_fs->support64bit() ) ibma |= (long) _gd.inode_bitmap_hi << 32;

			_belong_fs->read_data( ibma, (void *) _inode_bitmap, ims );
		}

	} // namespace ext4

} // namespace fs
