//
// Created by Li Shuang ( pseudonym ) on 2024-07-23 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "fs/ext4/ext4_inode.hh"
#include "fs/ext4/ext4_fs.hh"
#include "klib/template_algorithmn.hh"

namespace fs
{
	namespace ext4
	{
		Ext4IndexNode::Ext4IndexNode( Ext4Inode &node, Ext4FS * fs )
			: _inode( node )
			, _belong_fs( fs )
		{
			_cal_blocks();
			_cal_size();
		}

		Inode * Ext4IndexNode::lookup( eastl::string dirname )
		{
			if ( !( _inode.mode & ext4_imode_fdir ) )		// 当前inode不是目录
			{
				log_warn( "try to lookup an inode that's not dir" );
				return nullptr;
			}

			return nullptr;
		}

		Ext4Buffer * Ext4IndexNode::read_logical_block( long block, bool pin )
		{
			if ( _inode.flags.fl.extents )					// 使用extents方式索引
			{
				using ex_node = Ext4Inode::_block_u_t_::_extent_s_t_::_node_u_t_;

				Ext4ExtentHeader * ex_header = &_inode.blocks.extents.header;	// extent 头
				ex_node * st, * ed;												// (union) extent 中间节点或叶子节点
				st = &( _inode.blocks.extents.tree_nodes[ 0 ] );				// 节点数组起始元素
				ed = st + _inode.blocks.extents.header.valid_nodes_count - 1;	// 节点数组末尾元素

				Ext4Buffer * internal_block = nullptr;							// exTree 中间块的 buffer

				// 叶节点二分查找比较函数
				auto comp_leaf = [] ( Ext4ExtentLeafNode * mid, long * tar ) -> int
				{
					long t = *tar;
					long l = mid->logical_block_start, r = l + mid->length;
					if ( l <= t && t < r )
						return 0;
					else if ( t < l )
						return -1;
					else
						return 1;
				};
				// 中间节点二分查找比较函数
				auto comp_intr = [ & ] ( Ext4ExtentInternalNode * mid, long * tar ) -> int
				{
					long t = *tar;
					if ( t < mid->logical_block_start )
						return -1;
					if ( mid != &( ed->internal ) && t >= ( mid + 1 )->logical_block_start )
						return 1;
					else
						return 0;
				};

				while ( 1 )		// 查找 extent 树
				{
					if ( ex_header->depth == 0 )	// 已经到达叶节点
					{
						// 二分查找

						Ext4ExtentLeafNode * dst_node =
							klib::binary_search<Ext4ExtentLeafNode, long>(
								&( st->leaf ), &( ed->leaf ), &block, comp_leaf
							);
						if ( dst_node == nullptr )		// 查找失败
						{
							log_error( "ext4-inode : extents leaf binary search error" );
							return nullptr;
						}

						// 目标块的物理块号
						long phy_block = dst_node->start_lo + ( ( long ) dst_node->start_hi << 32 );
						phy_block += block - dst_node->logical_block_start;

						if ( internal_block )			// 返回前先将前一个块释放
							internal_block->unpin();

						return _belong_fs->read_block( phy_block, pin );
					}
					else							// 这是中间节点
					{
						// 二分查找

						Ext4ExtentInternalNode * next_node =
							klib::binary_search<Ext4ExtentInternalNode, long>(
								&( st->internal ), &( ed->internal ), &block, comp_intr
							);
						if ( next_node == nullptr )		// 查找失败
						{
							log_error( "ext4-inode : extents internal biary search error" );
							return nullptr;
						}

						long next_block = next_node->next_node_address;						// 下一个节点所在块号

						if ( internal_block )			// 读下一个块之前先将前一个块释放
							internal_block->unpin();

						internal_block = _belong_fs->read_block( next_block, true );
						ex_header = ( Ext4ExtentHeader * ) internal_block->get_data_ptr();
						st = ( ex_node * ) ( ex_header + 1 );
						ed = st + ex_header->valid_nodes_count - 1;
					}
				}
			}
			else											// 使用经典的直接/间接索引
			{
				if ( block < 12 )		// 直接索引
					return _belong_fs->read_block( _inode.blocks.blocks_ptr[ block ], pin );
				Ext4Buffer * buf;
				if ( block > _belong_fs->get_tind_block_start() )	// 三级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ tind_blocks_idx ], true );
					Ext4Buffer * res = _search_tindirect_block(
						block,
						_inode.blocks.blocks_ptr[ tind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				if ( block > _belong_fs->get_dind_block_start() )	// 二级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ dind_blocks_idx ], true );
					Ext4Buffer * res = _search_dindirect_block(
						block,
						_inode.blocks.blocks_ptr[ dind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				if ( block > _belong_fs->get_dind_block_start() )	// 一级索引
				{
					buf = _belong_fs->read_block( _inode.blocks.blocks_ptr[ sind_blocks_idx ], true );
					Ext4Buffer * res = _search_sindirect_block(
						block,
						_inode.blocks.blocks_ptr[ sind_blocks_idx ],
						buf->get_data_ptr(),
						pin
					);
					buf->unpin();
					return res;
				}
				log_error( "ext4-inode : 无法寻址的逻辑块号 %d", block );
				return nullptr;
			}
		}


		void Ext4IndexNode::_cal_blocks()
		{
			Ext4SuperBlock * ext4_sb = _belong_fs->get_ext4_superblock();
			if ( ext4_sb->feature_ro_compat.flags.huge_file )	// 启用 blocks_high
			{
				if ( _inode.flags.fl.huge_file )	// inode 中的 blocks 是标准 block 的数量
				{
					_has_blocks = _inode.blocks_lo + ( ( long ) _inode.blocks_high << 32 );
				}
				else	// inode 中的 blocks 是 512-bytes block 的数量
				{
					_has_blocks = _inode.blocks_lo + ( ( long ) _inode.blocks_high << 32 );
					_has_blocks *= 512;
					_has_blocks /= _belong_fs->rBlockSize();
				}
			}
			else	// inode 中的 blocks 是 512-bytes block 的数量
			{
				_has_blocks = _inode.blocks_lo * 512;
				_has_blocks /= _belong_fs->rBlockSize();
			}
		}

		void Ext4IndexNode::_cal_size()
		{
			if ( _belong_fs->support64bit() )
			{
				_has_size = _inode.size_lo + ( ( long ) _inode.size_high << 32 );
			}
			else
			{
				_has_size = _inode.size_lo;
			}
		}

		Ext4Buffer * Ext4IndexNode::_search_direct_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += target_block_no - start_block_no;
			return _belong_fs->read_block( *p_block, pin );
		}

		Ext4Buffer * Ext4IndexNode::_search_sindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_sind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_direct_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		Ext4Buffer * Ext4IndexNode::_search_dindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_dind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_sindirect_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		Ext4Buffer * Ext4IndexNode::_search_tindirect_block(
			long target_block_no,
			long start_block_no,
			void * index_block,
			bool pin )
		{
			u32 * p_block = ( u32 * ) index_block;
			p_block += _cal_tind_index( target_block_no, start_block_no );
			Ext4Buffer * buf = _belong_fs->read_block( *p_block, true );
			Ext4Buffer * res = _search_dindirect_block( target_block_no, *p_block, buf->get_data_ptr(), pin );
			buf->unpin();
			return res;
		}

		long Ext4IndexNode::_cal_tind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			unit *= unit * unit;		// block_size ^ 3
			return ( target_block - start_block ) / unit;
		}
		long Ext4IndexNode::_cal_dind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			unit *= unit;				// block_size ^ 2
			return ( target_block - start_block ) / unit;
		}
		long Ext4IndexNode::_cal_sind_index( long target_block, long start_block )
		{
			long unit = _belong_fs->rBlockSize();
			return ( target_block - start_block ) / unit;
		}

	} // namespace ext4

} // namespace fs
