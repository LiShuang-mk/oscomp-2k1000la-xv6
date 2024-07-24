//
// Created by Li Shuang ( pseudonym ) on 2024-07-22 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_fs.hh"
#include "fs/buffer_manager.hh"
#include "fs/dentrycache.hh"
#include "fs/dentry.hh"
#include "klib/common.hh"

#include <hsai_global.hh>
#include <device_manager.hh>
#include <block_device.hh>

namespace fs
{
	namespace ext4
	{
		Ext4FS::~Ext4FS()
		{
			delete _root_inode;
			delete[] _bgs;

			/// @todo how to release root dentry?
		}

		void Ext4FS::init( int dev, u64 start_lba, eastl::string fstype, eastl::string rootname, bool is_root )
		{
			if ( fstype != "ext4" )
			{
				log_error( "bad fs type" );
				return;
			}

			_lock.init( "ext4 fs" );

			// 获得 dev 设备的块大小

			hsai::VirtualDevice * vd = hsai::k_devm.get_device( ( uint ) dev );
			if ( vd->type() != hsai::dev_block )
			{
				log_error( "device %d is not a block device.", dev );
				return;
			}
			hsai::BlockDevice * bd = ( hsai::BlockDevice * ) vd;
			long blk_size = bd->get_block_size();

			// 通常块大小应当为 512 字节
			assert( blk_size == 512, "bad block size (%d bytes) from device %d", blk_size, dev );

			_owned_dev = dev;

			// 将 superblock 读出
			// ext4 的 superblock 位于分区偏移 1024 bytes

			Buffer buf;
			buf = k_bufm.read_sync( dev, 2 );
			new ( &_sb ) Ext4SB( ( Ext4SuperBlock * ) buf.get_data_ptr(), this );
			k_bufm.release_buffer_sync( buf );

			if ( start_lba % ( _sb.rBlockSize() / 512 ) != 0 )
			{
				log_warn( "分区起始LBA没有块大小对齐, 读写效率可能降低" );
			}

			// 初始化block缓存池
			new ( &_blocks_cacher ) Ext4BufferPool( _sb.rBlockSize(), this );

			// 计算 block group 数量

			_cache_blocks_per_group = _sb._super_block.blocks_per_group;
			_cache_inodes_per_group = _sb._super_block.inodes_per_group;
			log_info(
				"ext4\n"
				"\tblocks per group: %d"
				"\tinodes per group: %d",
				_cache_blocks_per_group,
				_cache_inodes_per_group
			);
			_cache_group_count = ( _sb.rBlockNum() + _cache_blocks_per_group - 1 ) / _cache_blocks_per_group;		// round up
			log_info( "ext4 block group count = %d", _cache_group_count );

			// 初始化块组描述符表

			_bgs = new Ext4BlockGroup[ _cache_group_count ];
			const int desc_per_block = _sb.rBlockSize() / sizeof( Ext4GroupDesc );
			long blk_gdt_cnt = 0;
			Ext4Buffer * pblk;
			for ( int i = 0; i < _cache_group_count; ++i )
			{
				int blk_gdt_off = i % desc_per_block;
				if ( blk_gdt_off == 0 )		// 遍历完一个sector中的desc后读取下一个sector
				{
					pblk = _blocks_cacher.request_block( 1 + blk_gdt_cnt );
					blk_gdt_cnt++;
				}

				Ext4GroupDesc * p_desc = ( Ext4GroupDesc * ) pblk->get_data_ptr();
				p_desc += blk_gdt_off;
				new ( &_bgs[ i ] ) Ext4BlockGroup( p_desc, this );
			}

			// 初始化 直接/间接索引块 起点

			long idx_per_block = rBlockSize() / 4;
			_s_indirect_block_start = 12;
			_d_indirect_block_start = _s_indirect_block_start + idx_per_block;
			_t_indirect_block_start = _d_indirect_block_start + idx_per_block * idx_per_block;

			// 初始化根节点

			long root_ino = ino_root;
			log_trace( "ext4-fs : init inode %d as root", root_ino );
			Ext4Inode node;
			if ( read_inode( root_ino, node ) < 0 )
			{
				log_panic( "ext4-fs : read inode failure while init root" );
				return;
			}
			_root_inode = new Ext4IndexNode( node, this );

			// 初始化根dentry

			_root_dir = dentrycache::k_dentryCache.alloDentry();
			if ( _root_dir == nullptr )
			{
				log_panic( "ext4-fs : init root dentry fail" );
				return;
			}
			new ( _root_dir ) fs::dentry( rootname, ( Inode * ) _root_inode, nullptr, true );
		}

		void Ext4FS::read_data( long block_no, void * dst, long size )
		{
			_lock.acquire();

			long b_siz = rBlockSize();						// 块大小

			u8 * d = ( u8* ) dst;							// 目标地址
			long b_idx = 0;									// 数据块索引
			u8 * f;											// 数据源地址
			long b_off;										// 块内偏移

			Ext4Buffer * blk_buf = nullptr;
			for ( long i = 0; i < size; i++ )
			{
				b_off = i % b_siz;
				if ( b_off == 0 )
				{
					_lock.release();
					if ( blk_buf != nullptr )
						blk_buf->unpin();				// unpin the buffer last read
					blk_buf = read_block( block_no + b_idx, true ); // pin the buffer
					f = ( u8* ) blk_buf->get_data_ptr();
					_lock.acquire();
					b_idx++;
				}
				d[ i ] = f[ b_off ];
			}

			if ( blk_buf != nullptr )
				blk_buf->unpin();

			_lock.release();
		}

		int Ext4FS::read_inode( long inode_no, Ext4Inode &node )
		{
			long bg = ( inode_no - 1 ) / _cache_inodes_per_group;
			long idx = ( inode_no - 1 ) % _cache_inodes_per_group;
			return _bgs[ bg ].read_inode( idx, node );
		}

	} // namespace ext4

} // namespace fs
